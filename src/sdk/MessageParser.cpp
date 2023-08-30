//
// Created by RGAA on 2023/8/11.
//

#include "MessageParser.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RLog.h"
#include "rgaa_opus_codec/OpusCodec.h"
#include "AudioPlayer.h"
#include "AppMessage.h"
#include "Context.h"
#include "sdk/RawImage.h"
#include "Statistics.h"
#include "SailfishSDK.h"
#include "MessageMaker.h"

#include <utility>

namespace rgaa {

    MessageParser::MessageParser(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk) {
        context_ = ctx;
        sdk_ = sdk;
        statistics_ = sdk_->GetStatistics();
    }

    std::shared_ptr<NetMessage> MessageParser::ParseMessage(const std::string& msg) {
        if (exit_) {
            return nullptr;
        }
        auto net_msg = std::make_shared<NetMessage>();
        if (!net_msg->ParseFromString(msg)) {
            return nullptr;
        }

        auto type = net_msg->type();
        if (type == MessageType::kVideoConfigSync) {

        }
        else if (type == MessageType::kVideoFrame) {
            // statistics begin
            auto& frame = net_msg->video_frame();
            auto current_time = GetCurrentTimestamp();
            if (last_recv_video_time == 0) {
                last_recv_video_time = current_time;
            }
            auto diff = current_time - last_recv_video_time;
            last_recv_video_time = current_time;
            statistics_->AppendVideoFrame(frame.data().size(), diff);

            statistics_->frame_index = frame.frame_index();
            statistics_->video_width = frame.width();
            statistics_->video_height = frame.height();
            if (frame.type() == VideoType::kH264) {
                statistics_->video_encode_format = "H264";
            }
            else if (frame.type() == VideoType::kH265) {
                statistics_->video_encode_format = "H265";
            }
            statistics_->AppendEncodeTime(frame.dup_idx(), frame.encode_time());

            auto network_time = frame.previous_network_time();
            statistics_->AppendNetworkTime(network_time);

            //LOGI("Network : {}", network_time);

            // statistics end

            if (sdk_->IsDebugging()) {
                context_->PostTask([=, this]() {
                    auto msg = MessageMaker::MakeACK(MessageType::kVideoFrame, net_msg->send_time(),
                                                     frame.frame_index());
                    sdk_->PostNetMessage(msg);
                });
            }

            if (video_frame_cbk_) {
                video_frame_cbk_(net_msg, net_msg->video_frame());
            }
        }
        else if (type == MessageType::kAudioConfigSync) {
            auto audio_config = net_msg->audio_config_sync();
        }
        else if (type == MessageType::kAudioFrame) {
            auto& frame = net_msg->audio_frame();
            int samples = frame.samples();
            int channels = frame.channels();
            int bits = frame.bits();
            if (!audio_decoder_) {
                audio_decoder_ = std::make_shared<Decoder>(samples, channels);
            }
            if (!audio_player_) {
                QMetaObject::invokeMethod(this, [=, this]() {
                    audio_player_ = std::make_shared<AudioPlayer>();
                    audio_player_->Init(samples, channels);
                });
                return net_msg;
            }

            const auto& audio_data = frame.data();
            std::vector<uint8_t> target_data;
            target_data.resize(audio_data.size());
            memcpy(target_data.data(), audio_data.data(), audio_data.size());

            statistics_->AppendAudioBytes(audio_data.size());
            statistics_->audio_channel_ = channels;
            statistics_->audio_samples_ = samples;
            statistics_->audio_bits_ = bits;

            std::vector<opus_int16> decoded_frame = audio_decoder_->Decode(target_data, frame.frame_size(), false);
            int bytes_size = decoded_frame.size() * 2;
            audio_player_->Write((char*)decoded_frame.data(), bytes_size);
        }
        else if (type == MessageType::kHeartBeat) {
            auto heart_beat = net_msg->heart_beat();
            //LOGI("Heart beat : {}", heart_beat.index());
        }
        else if (type == MessageType::kClipboard) {
            auto clipboard = net_msg->clipboard();
            auto text = clipboard.msg();
            auto msg = ClipboardMessage::Make(text);
            context_->SendAppMessage(msg);
        }
        else if (type == MessageType::kCursorInfo) {
            auto info = net_msg->cursor_info();
            if (cursor_cbk_) {
                auto raw_image = RawImage::MakeRGBA(const_cast<char *>(info.data().c_str()), info.data().size(), info.width(), info.height());
                cursor_cbk_(0, info.x(), info.y(), info.hotspot_x(), info.hotspot_y(), raw_image);
            }
        }

        return net_msg;
    }

    void MessageParser::SetOnVideoFrameCallback(OnVideoFrameCallback cbk) {
        video_frame_cbk_= std::move(cbk);
    }

    void MessageParser::SetOnCursorCallback(OnCursorCallback cbk) {
        cursor_cbk_ = std::move(cbk);
    }

    void MessageParser::Exit() {
        exit_ = true;
        if (audio_decoder_) {
            audio_decoder_.reset();
        }

    }

}