//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_SAILFISHSDK_H
#define SAILFISH_CLIENT_PC_SAILFISHSDK_H

#include <map>
#include <memory>
#include <functional>
#include <atomic>

#include "StreamItem.h"
#include "StreamConfig.h"
#include "Context.h"

namespace rgaa {

    class WSClient;
    class MessageParser;
    class FFmpegVideoDecoder;
    class Thread;
    class RawImage;
    class NetMessage;
    class Timer;
    class ClipboardManager;
    class Context;

    using OnVideoFrameDecodedCallback = std::function<void(int dup_idx, const std::shared_ptr<RawImage>&)>;
    using OnNetMessageCallback = std::function<void(const std::shared_ptr<NetMessage>&)>;

    class SailfishSDK : public std::enable_shared_from_this<SailfishSDK> {
    public:

        explicit SailfishSDK(const std::shared_ptr<Context> ctx, const StreamItem& item);
        ~SailfishSDK();

        void Init();
        void Exit();
        void PostNetMessage(const std::shared_ptr<NetMessage>& msg);
        void RegisterConfigCallback(OnNetMessageCallback&& cbk);
        void RegisterVideoFrameDecodedCallback(OnVideoFrameDecodedCallback cbk);

        StreamConfig GetStreamConfig();

    private:

        void InitTimers();
        void InitVideoDecoderIfNeeded(int dup_idx, int type, int width, int height);
        void HeartBeat();
        std::shared_ptr<FFmpegVideoDecoder> GetDecoderByDupIndex(int idx);
        void ParseConfig(const std::shared_ptr<NetMessage>& msg);

    private:

        std::shared_ptr<WSClient> ws_client_ = nullptr;
        std::shared_ptr<ClipboardManager> clipboard_manager_ = nullptr;
        std::shared_ptr<MessageParser> msg_parser_ = nullptr;

        std::shared_ptr<Thread> video_decoder_thread_ = nullptr;
        std::map<int, std::shared_ptr<FFmpegVideoDecoder>> video_decoders_;

        OnVideoFrameDecodedCallback video_frame_cbk_;
        OnNetMessageCallback config_msg_cbk_;

        std::shared_ptr<Timer> timer_ = nullptr;
        std::vector<size_t> timer_ids_;

        uint64_t heart_beat_idx_ = 0;

        StreamItem stream_item_;
        StreamConfig stream_config_;

        std::atomic_bool exit_ = false;

        std::shared_ptr<Context> context_ = nullptr;

        int clipboard_task_id_ = -1;

    };

}

#endif //SAILFISH_CLIENT_PC_SAILFISHSDK_H
