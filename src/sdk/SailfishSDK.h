//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_SAILFISHSDK_H
#define SAILFISH_CLIENT_PC_SAILFISHSDK_H

#include <memory>
#include <functional>

namespace rgaa {

    class WSClient;
    class MessageParser;
    class FFmpegVideoDecoder;
    class Thread;
    class RawImage;

    using OnVideoFrameDecodedCallback = std::function<void(const std::shared_ptr<RawImage>&)>;

    class SailfishSDK {
    public:

        SailfishSDK();
        ~SailfishSDK();

        void Init();

        void RegisterVideoFrameDecodedCallback(OnVideoFrameDecodedCallback cbk);

    private:

        void InitVideoDecoderIfNeeded(int type, int width, int height);

    private:

        std::shared_ptr<WSClient> ws_client_ = nullptr;

        std::shared_ptr<MessageParser> msg_parser_ = nullptr;

        std::shared_ptr<Thread> video_decoder_thread_ = nullptr;
        std::shared_ptr<FFmpegVideoDecoder> video_decoder_ = nullptr;

        OnVideoFrameDecodedCallback video_frame_cbk_;

    };

}

#endif //SAILFISH_CLIENT_PC_SAILFISHSDK_H
