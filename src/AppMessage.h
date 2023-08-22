//
// Created by RGAA on 2023-08-17.
//

#ifndef SAILFISH_CLIENT_PC_APPMESSAGE_H
#define SAILFISH_CLIENT_PC_APPMESSAGE_H

#include <utility>

#include "rgaa_common/RMessage.h"
#include "sdk/StreamItem.h"

namespace rgaa {

    constexpr int kCodeTimeout1S = 0x1001;
    constexpr int kCodeStreamAdded = 0x1002;
    constexpr int kCodeCloseWorkspace = 0x1003;
    constexpr int kCodeClearWorkspace = 0x1004;
    constexpr int kCodeClipboard = 0x1005;
    constexpr int kCodeStreamUpdated = 0x1006;

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

    // Stream will be updated
    class StreamItemUpdated : public Message {
    public:
        static std::shared_ptr<StreamItemUpdated> Make(const StreamItem& item) {
            return std::make_shared<StreamItemUpdated>(item);
        }

        explicit StreamItemUpdated(const StreamItem& item) : Message(kCodeStreamUpdated) {
            this->item_ = item;
        }

    public:

        StreamItem item_;
    };

    // Close workspace
    class CloseWorkspace : public Message {
    public:
        static std::shared_ptr<CloseWorkspace> Make(const StreamItem& item) {
            return std::make_shared<CloseWorkspace>(item);
        }
        explicit CloseWorkspace(const StreamItem& item) : Message(kCodeCloseWorkspace) {
            this->item_ = item;
        }

    public:
        StreamItem item_;
    };

    // Close workspace
    class ClearWorkspace : public Message {
    public:
        static std::shared_ptr<ClearWorkspace> Make(const StreamItem& item) {
            return std::make_shared<ClearWorkspace>(item);
        }
        explicit ClearWorkspace(const StreamItem& item) : Message(kCodeClearWorkspace) {
            this->item_ = item;
        }

    public:
        StreamItem item_;
    };

    // Clipboard
    class ClipboardMessage : public Message {
    public:
        static std::shared_ptr<ClipboardMessage> Make(const std::string& msg) {
            return std::make_shared<ClipboardMessage>(msg);
        }

        explicit ClipboardMessage(std::string  msg) : Message(kCodeClipboard), msg_(std::move(msg)) {}

    public:
        std::string msg_;
    };

}

#endif //SAILFISH_CLIENT_PC_APPMESSAGE_H
