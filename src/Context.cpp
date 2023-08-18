//
// Created by RGAA on 2023-08-11.
//

#include "Context.h"

#include "sdk/StreamDBManager.h"
#include "rgaa_common/RMessageQueue.h"
#include "rgaa_common/RThread.h"

namespace rgaa {

    Context::Context() {
        db_mgr_ = std::make_shared<StreamDBManager>();
        msg_queue_ = std::make_shared<MessageQueue>();
        msg_thread_ = std::make_shared<Thread>([=, this]() {
            msg_queue_->Poll();
        }, "msg thread", false);
    }

    Context::~Context() {
        msg_queue_->Exit();

        if (msg_thread_ && msg_thread_->IsJoinable()) {
            msg_thread_->Join();
        }
    }

    std::shared_ptr<StreamDBManager> Context::GetDBManager() {
        return db_mgr_;
    }

}