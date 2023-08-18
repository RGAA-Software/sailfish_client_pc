//
// Created by RGAA on 2023-08-17.
//

#ifndef SAILFISH_CLIENT_PC_APPMESSAGE_H
#define SAILFISH_CLIENT_PC_APPMESSAGE_H

#include "rgaa_common/RMessage.h"

namespace rgaa {

    constexpr int kCodeTimeout1S = 0x1001;

    class Timeout1S : public Message {
    public:

        explicit Timeout1S(int c) : Message(c) {}

    };

}

#endif //SAILFISH_CLIENT_PC_APPMESSAGE_H
