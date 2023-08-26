//
// Created by RGAA on 2023/8/11.
//

#ifndef SAILFISH_CLIENT_PC_FFMPEGVIDEODECODER_H
#define SAILFISH_CLIENT_PC_FFMPEGVIDEODECODER_H

extern "C" {

    #include <libavcodec/codec.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
}

#include <memory>
#include <functional>

namespace rgaa {

    class Data;
    class RawImage;
    class SailfishSDK;

    class FFmpegVideoDecoder {
    public:

        explicit FFmpegVideoDecoder(const std::shared_ptr<SailfishSDK>& sdk, bool to_rgb);
        ~FFmpegVideoDecoder();

        int Init(int codec_type, int width, int height);
        std::shared_ptr<RawImage> Decode(const std::shared_ptr<Data>& frame);
        void Release();

        bool NeedReConstruct(int codec_type, int width, int height);

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        int codec_type_ = -1;
        int frame_width_ = 0;
        int frame_height_ = 0;
        bool stop_ = false;
        AVCodecContext* codec_context = nullptr;
        AVCodec* codec = nullptr;
        AVPacket* packet = nullptr;
        AVFrame* av_frame = nullptr;

        std::shared_ptr<RawImage> decoded_image_ = nullptr;

        bool cvt_to_rgb = false;
    };

}

#endif //SAILFISH_CLIENT_PC_FFMPEGVIDEODECODER_H
