//
// Created by RGAA on 2023/8/11.
//

#ifndef SAILFISH_CLIENT_PC_MESSAGEPARSER_H
#define SAILFISH_CLIENT_PC_MESSAGEPARSER_H

#include "messages.pb.h"

namespace rgaa {

    using OnVideoFrameCallback = std::function<void(const std::shared_ptr<NetMessage>&, const VideoFrameSync&)>;

    class MessageParser {
    public:

        void ParseMessage(const std::string& msg);

        void SetOnVideoFrameCallback(OnVideoFrameCallback cbk);

    private:

        OnVideoFrameCallback video_frame_cbk_;

    };

}

#endif //SAILFISH_CLIENT_PC_MESSAGEPARSER_H
