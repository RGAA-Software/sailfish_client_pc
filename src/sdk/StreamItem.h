//
// Created by RGAA on 2023/8/14.
//

#ifndef SAILFISH_CLIENT_PC_STREAMITEM_H
#define SAILFISH_CLIENT_PC_STREAMITEM_H

#include <string>

namespace rgaa {

    class StreamItem {
    public:

        int _id = 0;

        // stream id
        std::string stream_id_;

        // stream name
        std::string stream_name_;

    };

}

#endif //SAILFISH_CLIENT_PC_STREAMITEM_H
