//
// Created by RGAA on 2023-08-17.
//

#ifndef SAILFISH_CLIENT_PC_APPMESSAGE_H
#define SAILFISH_CLIENT_PC_APPMESSAGE_H

#include "rgaa_common/RMessage.h"
#include "sdk/StreamItem.h"

namespace rgaa {

    constexpr int kCodeTimeout1S = 0x1001;
    constexpr int kCodeStreamAdded = 0x1002;
    constexpr int kCodeCloseWorkspace = 0x1003;

    class Timeout1S : public Message {
    public:
        static std::shared_ptr<Timeout1S> Make() {
            return std::make_shared<Timeout1S>(kCodeTimeout1S);
        }

        explicit Timeout1S(int c) : Message(c) {}
    };

    // Stream item will be added in to database
    class StreamItemAdded : public Message {
    public:
        static std::shared_ptr<StreamItemAdded> Make(const StreamItem& item) {
            return std::make_shared<StreamItemAdded>(item);
        }

        explicit StreamItemAdded(const StreamItem& item) : Message(kCodeStreamAdded) {
            this->item_ = item;
        }

    public:

        StreamItem item_;
    };

    // Close workspace
    class CloseWorkspace : public Message {
    public:

        static std::shared_ptr<CloseWorkspace> Make() {
            return std::make_shared<CloseWorkspace>();
        }

        explicit CloseWorkspace() : Message(kCodeCloseWorkspace) {

        }

    };

}

#endif //SAILFISH_CLIENT_PC_APPMESSAGE_H
