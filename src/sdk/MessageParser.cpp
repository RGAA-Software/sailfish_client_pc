//
// Created by RGAA on 2023/8/11.
//

#include "MessageParser.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RLog.h"
#include "rgaa_opus_codec/OpusCodec.h"
#include "AudioPlayer.h"

#include <utility>

namespace rgaa {

    void MessageParser::ParseMessage(const std::string& msg) {
        auto net_msg = std::make_shared<NetMessage>();
        if (!net_msg->ParseFromString(msg)) {
            return;
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
                return;
            }

            const auto& audio_data = frame.data();
            std::vector<uint8_t> target_data;
            target_data.resize(audio_data.size());
            memcpy(target_data.data(), audio_data.data(), audio_data.size());

            std::vector<opus_int16> decoded_frame = audio_decoder_->Decode(target_data, frame.frame_size(), false);
            int bytes_size = decoded_frame.size() * 2;
            audio_player_->Write((char*)decoded_frame.data(), bytes_size);
        }

    }

    void MessageParser::SetOnVideoFrameCallback(OnVideoFrameCallback cbk) {
        video_frame_cbk_= std::move(cbk);
    }

    void MessageParser::Exit() {
        if (audio_decoder_) {
            audio_decoder_.reset();
        }

    }

}