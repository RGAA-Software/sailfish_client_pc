//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_SAILFISHSDK_H
#define SAILFISH_CLIENT_PC_SAILFISHSDK_H

#include <map>
#include <memory>
#include <functional>

namespace rgaa {

    class WSClient;
    class MessageParser;
    class FFmpegVideoDecoder;
    class Thread;
    class RawImage;
    class NetMessage;
    class Timer;

    using OnVideoFrameDecodedCallback = std::function<void(int dup_idx, const std::shared_ptr<RawImage>&)>;

    class SailfishSDK {
    public:

        SailfishSDK();
        ~SailfishSDK();

        void Init();
        void Exit();
        void PostNetMessage(const std::shared_ptr<NetMessage>& msg);
        void RegisterVideoFrameDecodedCallback(OnVideoFrameDecodedCallback cbk);

    private:

        void InitTimers();
        void InitVideoDecoderIfNeeded(int dup_idx, int type, int width, int height);
        void HeartBeat();
        std::shared_ptr<FFmpegVideoDecoder> GetDecoderByDupIndex(int idx);

    private:

        std::shared_ptr<WSClient> ws_client_ = nullptr;

        std::shared_ptr<MessageParser> msg_parser_ = nullptr;

        std::shared_ptr<Thread> video_decoder_thread_ = nullptr;
        std::map<int, std::shared_ptr<FFmpegVideoDecoder>> video_decoders_;

        OnVideoFrameDecodedCallback video_frame_cbk_;

        std::shared_ptr<Timer> timer_ = nullptr;
        std::vector<size_t> timer_ids_;

        uint64_t heart_beat_idx_ = 0;

    };

}

#endif //SAILFISH_CLIENT_PC_SAILFISHSDK_H
