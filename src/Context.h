//
// Created by RGAA on 2023-08-11.
//

#ifndef SAILFISH_CLIENT_PC_CONTEXT_H
#define SAILFISH_CLIENT_PC_CONTEXT_H

#include <memory>

namespace rgaa {

    class StreamDBManager;
    class MessageQueue;
    class Thread;

    class Context {
    public:

        Context();
        ~Context();

        std::shared_ptr<StreamDBManager> GetDBManager();

    private:

        std::shared_ptr<StreamDBManager> db_mgr_ = nullptr;
        std::shared_ptr<MessageQueue> msg_queue_ = nullptr;
        std::shared_ptr<Thread> msg_thread_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_CONTEXT_H
