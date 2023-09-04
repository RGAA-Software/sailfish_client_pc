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
#include "rgaa_common/RTimer.h"
#include "RawImage.h"
#include "messages.pb.h"
#include "MessageMaker.h"
#include "StreamItem.h"
#include "ClipboardManager.h"
#include "rgaa_common/RMessageQueue.h"
#include "AppMessage.h"
#include "Statistics.h"
#include "src/Settings.h"

namespace rgaa {

    SailfishSDK::SailfishSDK(const std::shared_ptr<Context>& ctx, const StreamItem& item) {
        context_ = ctx;
        stream_item_ = item;
        statistics_ = std::make_shared<Statistics>();
        settings_ = Settings::Instance();
    }

    SailfishSDK::~SailfishSDK() {

    }

    void SailfishSDK::Init() {
        video_decoder_thread_ = Thread::Make("video_decoder", 128);
        video_decoder_thread_->Poll();

        clipboard_manager_ = std::make_shared<ClipboardManager>(shared_from_this());
        clipboard_manager_->Init();

        InitTimers();

        msg_parser_ = std::make_shared<MessageParser>(context_, shared_from_this());
        msg_parser_->SetOnVideoFrameCallback([this](const std::shared_ptr<NetMessage>& msg, const VideoFrameSync& frame) {
            InitVideoDecoderIfNeeded(frame.dup_idx(), frame.type(), frame.width(), frame.height());
            auto data = Data::Make(frame.data().data(), frame.data().size());
            std::shared_ptr<FFmpegVideoDecoder> decoder = GetDecoderByDupIndex(frame.dup_idx());
            video_decoder_thread_->Post(SimpleThreadTask::Make([=, this]() {
                auto raw_image = decoder->Decode(data);
                if (raw_image && video_frame_cbk_ && !exit_) {
                    video_frame_cbk_(frame.dup_idx(), raw_image);
                }
            }));
        });

        LOGI("Will connect to {} {} ", stream_item_.stream_host, stream_item_.stream_port);
        ws_client_ = std::make_shared<WSClient>(stream_item_);
        ws_client_->SetOnMessageCallback([this](const std::string& msg) {
            auto net_msg = msg_parser_->ParseMessage(msg);
            if (!net_msg) {
                return;
            }

            if (net_msg->has_config()) {
                ParseConfig(net_msg);
                if (config_msg_cbk_) {
                    config_msg_cbk_(net_msg);
                }

                this->ReportConfig();
            }
        });

        clipboard_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeClipboard, [=, this](auto& msg) {
            auto target_msg = std::dynamic_pointer_cast<ClipboardMessage>(msg);
            if (!target_msg) {return;}
            clipboard_manager_->SetText(target_msg->msg_.c_str());
            LOGI("clipboard setText : {}", target_msg->msg_);
        }));

        ws_client_->Connect();
    }

    void SailfishSDK::InitTimers() {
        timer_ = std::make_shared<Timer>();
        auto timer_1s_id = timer_->add(std::chrono::milliseconds(10), [=, this](rgaa::timer_id id) {
            HeartBeat();

            statistics_->streaming_time++;
        }, std::chrono::seconds(1));
        timer_ids_.push_back(timer_1s_id);
    }

    void SailfishSDK::InitVideoDecoderIfNeeded(int dup_idx, int type, int width, int height) {
        auto func_create_decoder = [=, this]() {
            auto video_decoder = std::make_shared<FFmpegVideoDecoder>(shared_from_this(), false);
            video_decoder->Init(type, width, height);
            video_decoders_.insert(std::make_pair(dup_idx, video_decoder));
        };

        std::shared_ptr<FFmpegVideoDecoder> decoder = GetDecoderByDupIndex(dup_idx);

        if (!decoder) {
            func_create_decoder();
        }
        else if (decoder->NeedReConstruct(type, width, height)) {
            decoder->Release();
            decoder = nullptr;
            func_create_decoder();
        }

    }

    std::shared_ptr<FFmpegVideoDecoder> SailfishSDK::GetDecoderByDupIndex(int dup_idx) {
        for (const auto& [idx, dc] : video_decoders_) {
            if (dup_idx == idx) {
                return dc;
            }
        }
        return nullptr;
    }

    void SailfishSDK::PostNetMessage(const std::shared_ptr<NetMessage>& msg) {
        if (!ws_client_) {
            return;
        }
        auto msg_buffer = msg->SerializeAsString();
        ws_client_->PostBinaryMessage(msg_buffer);
    }

    void SailfishSDK::HeartBeat() {
        if (ws_client_ && !exit_ ) {
            heart_beat_idx_++;
            auto msg = MessageMaker::MakeHeartBeat(heart_beat_idx_);
            auto payload = msg->SerializeAsString();
            ws_client_->PostBinaryMessage(payload);
        }
    }

    void SailfishSDK::RegisterVideoFrameDecodedCallback(OnVideoFrameDecodedCallback cbk) {
        video_frame_cbk_ = std::move(cbk);
    }

    void SailfishSDK::RegisterConfigCallback(OnNetMessageCallback&& cbk) {
        config_msg_cbk_ = std::move(cbk);
    }

    void SailfishSDK::ParseConfig(const std::shared_ptr<NetMessage>& msg) {
        const auto& config = msg->config();
        stream_config_.screen_size = config.screen_size();
    }

    StreamConfig SailfishSDK::GetStreamConfig() {
        return stream_config_;
    }

    std::shared_ptr<MessageParser> SailfishSDK::GetMsgParser() {
        return msg_parser_;
    }

    std::shared_ptr<Statistics> SailfishSDK::GetStatistics() {
        return statistics_;
    }

    void SailfishSDK::EnableClipboard() {
        clipboard_manager_->Enable();
        PostNetMessage(MessageMaker::MakeClipboardStatus(true));
    }

    void SailfishSDK::DisableClipboard() {
        clipboard_manager_->Disable();
        PostNetMessage(MessageMaker::MakeClipboardStatus(false));
    }

    void SailfishSDK::EnableAudio() {
        PostNetMessage(MessageMaker::MakeAudioStatus(true));
    }

    void SailfishSDK::DisableAudio() {
        PostNetMessage(MessageMaker::MakeAudioStatus(false));
    }

    void SailfishSDK::StartDebug() {
        debug_ = true;
        PostNetMessage(MessageMaker::MakeDebugStatus(true));
    }

    void SailfishSDK::StopDebug() {
        debug_ = false;
        PostNetMessage(MessageMaker::MakeDebugStatus(false));
    }

    bool SailfishSDK::IsDebugging() {
        return debug_;
    }

    void SailfishSDK::ReportConfig() {
        auto audio_on = settings_->IsAudioEnabled();
        if (audio_on) {
            EnableAudio();
        }
        else {
            DisableAudio();
        }

        auto clipboard_on = settings_->IsClipboardEnabled();
        if (clipboard_on) {
            EnableClipboard();
        }
        else {
            DisableClipboard();
        }
    }

    void SailfishSDK::Exit() {
        exit_ = true;
        if (clipboard_manager_) {
            clipboard_manager_->Exit();
        }

        context_->RemoveMessageTask(clipboard_task_id_);

        for (const auto& id : timer_ids_) {
            timer_->remove(id);
        }
        if (ws_client_) {
            ws_client_->Exit();
        }
        for (const auto& [idx, dc] : video_decoders_) {
            if (dc) {
                dc->Release();
                LOGI("Exit video decoder");
            }
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
