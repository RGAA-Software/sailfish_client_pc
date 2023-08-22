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

#include <utility>

namespace rgaa {

    MessageParser::MessageParser(const std::shared_ptr<Context>& ctx) {
        context_ = ctx;
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