//
// Created by RGAA on 2023-08-10.
//

#include "SailfishSDK.h"

#include "WSClient.h"
#include "MessageParser.h"
#include "FFmpegVideoDecoder.h"
#include "rgaa_common/RThread.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RData.h"
#include "RawImage.h"
#include "messages.pb.h"

namespace rgaa {

    SailfishSDK::SailfishSDK() {

    }

    SailfishSDK::~SailfishSDK() {

    }

    void SailfishSDK::Init() {
        video_decoder_thread_ = Thread::Make("video_decoder", 128);
        video_decoder_thread_->Poll();

        msg_parser_ = std::make_shared<MessageParser>();
        msg_parser_->SetOnVideoFrameCallback([this](const std::shared_ptr<NetMessage>& msg, const VideoFrameSync& frame) {
            InitVideoDecoderIfNeeded(frame.type(), frame.width(), frame.height());
            auto data = Data::Make(frame.data().data(), frame.data().size());
            video_decoder_thread_->Post(SimpleThreadTask::Make([=, this]() {
                auto raw_image = video_decoder_->Decode(data);
                if (raw_image && video_frame_cbk_) {
                    video_frame_cbk_(raw_image);
                    //LOGI("Decode success: {} x {}", raw_image->img_width, raw_image->img_height);
                }
            }));
        });

//        ws_client_ = std::make_shared<WSClient>("127.0.0.1", 9090);
//        ws_client_ = std::make_shared<WSClient>("192.168.10.130", 9090);
        ws_client_ = std::make_shared<WSClient>("10.0.0.67", 9090);
//        ws_client_ = std::make_shared<WSClient>("10.0.0.70", 9090);
        ws_client_->SetOnMessageCallback([this](const std::string& msg) {
            msg_parser_->ParseMessage(msg);
        });

        ws_client_->Connect();
    }

    void SailfishSDK::InitVideoDecoderIfNeeded(int type, int width, int height) {
        auto func_create_decoder = [=, this]() {
            video_decoder_ = std::make_shared<FFmpegVideoDecoder>(false);
            video_decoder_->Init(type, width, height);
        };
        if (!video_decoder_) {
            func_create_decoder();
        }
        else if (video_decoder_->NeedReConstruct(type, width, height)) {
            video_decoder_->Release();
            video_decoder_ = nullptr;
            func_create_decoder();
        }

    }

    void SailfishSDK::PostNetMessage(const std::shared_ptr<NetMessage>& msg) {
        if (!ws_client_) {
            return;
        }
        auto msg_buffer = msg->SerializeAsString();
        ws_client_->PostBinaryMessage(msg_buffer);
    }

    void SailfishSDK::RegisterVideoFrameDecodedCallback(OnVideoFrameDecodedCallback cbk) {
        video_frame_cbk_ = std::move(cbk);
    }

    void SailfishSDK::Exit() {
        if (ws_client_) {
            ws_client_->Exit();
        }
        if (video_decoder_) {
            video_decoder_->Release();
            LOGI("Exit video decoder");
        }
        if (video_decoder_thread_) {
            video_decoder_thread_->Exit();
            LOGI("Exit video decoder thread");
        }
        if (msg_parser_) {
            msg_parser_->Exit();
            LOGI("Exit video msg parser");
        }
    }
}
