//
// Created by RGAA on 2023-08-11.
//

#include "Context.h"

#include "sdk/StreamDBManager.h"
#include "rgaa_common/RMessageQueue.h"
#include "rgaa_common/RThread.h"

namespace rgaa {

    Context::Context() : QObject(nullptr) {
        db_mgr_ = std::make_shared<StreamDBManager>();
        msg_queue_ = std::make_shared<MessageQueue>();
        msg_thread_ = std::make_shared<Thread>([=, this]() {
            msg_queue_->PollBlocked();
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

    void Context::PostTask(std::function<void()>&& task) {
        task_thread_->Post(SimpleThreadTask::Make(std::move(task)));
    }

    void Context::PostUITask(std::function<void()>&& task) {
        QMetaObject::invokeMethod(this, [=, this]() {
            task();
        });
    }

    int Context::RegisterMessageTask(const std::shared_ptr<MessageTask>& task) {
        if (msg_queue_) {
            return msg_queue_->RegisterTask(task);
        }
        return -1;
    }

    void Context::RemoveMessageTask(int task_id) {
        if (msg_queue_) {
            msg_queue_->RemoveTask(task_id);
        }
    }

    void Context::SendAppMessage(const std::shared_ptr<Message>& msg) {
        if (msg_queue_) {
            msg_queue_->Queue(msg);
        }
    }

}