//
// Created by RGAA on 2023/8/11.
//

#include "FFmpegVideoDecoder.h"

#include "RawImage.h"
#include "messages.pb.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RCloser.h"
#include "Statistics.h"

#include <libyuv.h>
#include <iostream>

namespace rgaa {

    FFmpegVideoDecoder::FFmpegVideoDecoder(bool to_rgb) {
        this->cvt_to_rgb = to_rgb;
    }

    FFmpegVideoDecoder::~FFmpegVideoDecoder() {

    }

    int FFmpegVideoDecoder::Init(int codec_type, int width, int height) {
        auto format_num = [](int val) -> int {
            auto t = val % 2;
            return val + t;
        };

        this->codec_type_ = codec_type;
        this->frame_width_ = format_num(width);
        this->frame_height_ = format_num(height);
        AVCodecID codec_id = AV_CODEC_ID_NONE;
        if (codec_type == VideoType::kH264) {
            codec_id = AV_CODEC_ID_H264;
        }
        else if (codec_type == VideoType::kH265) {
            codec_id = AV_CODEC_ID_H265;
        }
        /*else if (codec_type == VideoType::kVp9) {
            codec_id = AV_CODEC_ID_VP9;
        }*/
        if (codec_id == AV_CODEC_ID_NONE) {
            std::cout << "CodecId not find : " << codec_type << std::endl;
            return -1;
        }

        codec = const_cast<AVCodec*>(avcodec_find_decoder(codec_id));
        if (!codec) {
            printf("can not find H265 codec\n");
            return -1;
        }

        codec_context = avcodec_alloc_context3(codec);
        if (codec_context == NULL) {
            printf("Could not alloc video context!\n");
            return -1;
        }

        AVCodecParameters* codec_params = avcodec_parameters_alloc();
        if (avcodec_parameters_from_context(codec_params, codec_context) < 0) {
            printf("Failed to copy av codec parameters from codec context.");
            avcodec_parameters_free(&codec_params);
            avcodec_free_context(&codec_context);
            return -1;
        }

        if (!codec_params) {
            printf("Source codec context is NULL.");
            return -1;
        }
        codec_context->thread_count = (int)std::thread::hardware_concurrency()/2;
        codec_context->thread_type = FF_THREAD_SLICE;
        if (avcodec_open2(codec_context, codec, NULL) < 0) {
            printf("Failed to open decoder");
            Release();
            return -1;
        }

        std::cout << "thread count : " << codec_context->thread_count << std::endl;

        av_init_packet(&packet);
        av_frame = av_frame_alloc();

        avcodec_parameters_free(&codec_params);
        return 0;
    }

    static void I420ToRGB24(unsigned char* yuvData, unsigned char* rgb24, int width, int height) {

        unsigned char* ybase = yuvData;
        unsigned char* ubase = &yuvData[width * height];
        unsigned char* vbase = &yuvData[width * height * 5 / 4];
        libyuv::I420ToRGB24(ybase, width, ubase, width / 2, vbase, width / 2,
                            rgb24,
                            width * 3, width, height);

    }

    std::shared_ptr<RawImage> FFmpegVideoDecoder::Decode(const std::shared_ptr<Data>& frame) {
        if (!codec_context || !av_frame || stop_) {
            return nullptr;
        }

        auto decode_usage = RegionTimeCount::Make([](uint64_t count) {
            Statistics::Instance()->AppendVideoDecode(count);
        });

        av_frame_unref(av_frame);

        packet.data = (uint8_t*)frame->CStr();
        packet.size = frame->Size();

        auto format = codec_context->pix_fmt;

        int ret = avcodec_send_packet(codec_context, &packet);
        if (ret != 0) {
            return nullptr;
        }

        ret = avcodec_receive_frame(codec_context, av_frame);
        if (ret != 0) {
            return nullptr;
        }

        auto width = av_frame->width;
        auto height = av_frame->height;

        auto x1 = av_frame->linesize[0];
        auto x2 = av_frame->linesize[1];
        auto x3 = av_frame->linesize[2];
        width = x1;

        if (format == AVPixelFormat::AV_PIX_FMT_YUV420P || format == AVPixelFormat::AV_PIX_FMT_NV12) {
            //std::cout << "width : " << width << " height : " << height << " frame width : " << frame_width_ << " frame height : " << frame_height_ << std::endl;
            //image = RawImage::MakeI420(nullptr, width * height * 1.5, width, height);
            //char* buffer = image->Data();
            //memcpy(buffer, av_frame->data[0], width * height);
            //memcpy(buffer + (width * height), av_frame->data[1], width * height / 4);
            //memcpy(buffer + (width * height * 5 / 4), av_frame->data[2], width * height / 4);

            frame_width_ = std::min(frame_width_, width);
            frame_height_ = std::min(frame_height_, height);
            if (!decoded_image_ || frame_width_ != decoded_image_->img_width || frame_height_ != decoded_image_->img_height) {
                decoded_image_ = RawImage::MakeI420(nullptr, frame_width_ * frame_height_ * 1.5, frame_width_, frame_height_);
            }
            char* buffer = decoded_image_->Data();
            for (int i = 0; i < frame_height_; i++) {
                memcpy(buffer + frame_width_*i, av_frame->data[0] + width*i, frame_width_);
            }

            int y_offset = frame_width_ * frame_height_;
            for (int j = 0; j < frame_height_ / 2; j++) {
                memcpy(buffer + y_offset + (frame_width_/2 * j), av_frame->data[1] + width/2 * j, frame_width_/2);
            }

            int yu_offset = y_offset + (frame_width_ / 2) * (frame_height_ / 2);
            for (int k = 0; k < frame_height_ / 2; k++) {
                memcpy(buffer + yu_offset + (frame_width_/2 * k), av_frame->data[2] + width/2 * k, frame_width_/2);
            }
        }

        if (decoded_image_ && !stop_) {
            if (cvt_to_rgb) {
                auto rgb = RawImage::MakeRGB(nullptr, frame_width_ * frame_height_ * 3, frame_width_, frame_height_);
                auto rgb_data = rgb->Data();
                auto yuv_data = decoded_image_->Data();
                I420ToRGB24((uint8_t*)yuv_data, (uint8_t*)rgb_data, frame_width_, frame_height_);
                return rgb;
            }
        }
        return decoded_image_;
    }

    void FFmpegVideoDecoder::Release() {
        stop_ = true;
        if (codec_context != nullptr) {
            avcodec_free_context(&codec_context);
            codec_context = nullptr;
        }

        if (av_frame != nullptr) {
            av_packet_unref(&packet);
            av_free(av_frame);
            av_frame = nullptr;
        }

        av_packet_unref(&packet);
    }

    bool FFmpegVideoDecoder::NeedReConstruct(int codec_type, int width, int height) {
        return codec_type != this->codec_type_ || width != this->frame_width_ || height != this->frame_height_;
    }

}