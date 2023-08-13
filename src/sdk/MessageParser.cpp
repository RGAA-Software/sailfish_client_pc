//
// Created by RGAA on 2023/8/11.
//

#include "MessageParser.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RLog.h"

#include <utility>

namespace rgaa {

    void MessageParser::ParseMessage(const std::string& msg) {
        auto net_msg = std::make_shared<NetMessage>();
        if (!net_msg->ParseFromString(msg)) {
            return;
        }

        auto type = net_msg->type();
        if (type == MessageType::kVideoFrame) {
            if (video_frame_cbk_) {
                // local pc test
                //auto current_time = GetCurrentTimestamp();
                //auto diff = current_time - net_msg->send_time();
                //LOGI("Local pc network diff : {}", diff);

                video_frame_cbk_(net_msg, net_msg->video_frame());
            }
        }

    }

    void MessageParser::SetOnVideoFrameCallback(OnVideoFrameCallback cbk) {
        video_frame_cbk_= std::move(cbk);
    }

}