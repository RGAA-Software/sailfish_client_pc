//
// Created by RGAA on 2023/8/11.
//

#ifndef SAILFISH_CLIENT_PC_MESSAGEPARSER_H
#define SAILFISH_CLIENT_PC_MESSAGEPARSER_H

#include <QObject>
#include <memory>
#include <atomic>

#include "messages.pb.h"

namespace rgaa {

    class AudioPlayer;
    class Decoder;
    class Context;
    class RawImage;

    using OnVideoFrameCallback = std::function<void(const std::shared_ptr<NetMessage>&, const VideoFrameSync&)>;
    using OnCursorCallback = std::function<void(int dup_idx, int x, int y, int hpx, int hpy, const std::shared_ptr<RawImage>&)>;


    class MessageParser : public QObject {
    public:

        explicit MessageParser(const std::shared_ptr<Context>& ctx);

        std::shared_ptr<NetMessage> ParseMessage(const std::string& msg);

        void SetOnVideoFrameCallback(OnVideoFrameCallback cbk);
        void SetOnCursorCallback(OnCursorCallback cbk);

        void Exit();

    private:

        OnVideoFrameCallback video_frame_cbk_;
        OnCursorCallback cursor_cbk_;

        std::shared_ptr<Decoder> audio_decoder_ = nullptr;
        std::shared_ptr<AudioPlayer> audio_player_ = nullptr;
        std::shared_ptr<Context> context_ = nullptr;

        bool exit_ = false;

    };

}

#endif //SAILFISH_CLIENT_PC_MESSAGEPARSER_H
