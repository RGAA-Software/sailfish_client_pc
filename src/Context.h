//
// Created by RGAA on 2023-08-11.
//

#ifndef SAILFISH_CLIENT_PC_CONTEXT_H
#define SAILFISH_CLIENT_PC_CONTEXT_H

#include <memory>
#include <functional>

#include <QtCore/QObject>
#include <QtCore/QMetaObject>

namespace rgaa {

    class StreamDBManager;
    class MessageQueue;
    class Thread;
    class Message;
    class MessageTask;

    class Context : public QObject {
    Q_OBJECT
    public:

        Context();
        ~Context();

        std::shared_ptr<StreamDBManager> GetDBManager();

        void SendAppMessage(const std::shared_ptr<Message>& msg);
        int RegisterMessageTask(const std::shared_ptr<MessageTask>& task);
        void RemoveMessageTask(int task_id);

        void PostTask(std::function<void()>&& task);
        void PostUITask(std::function<void()>&& task);

    private:

        std::shared_ptr<StreamDBManager> db_mgr_ = nullptr;
        std::shared_ptr<MessageQueue> msg_queue_ = nullptr;
        std::shared_ptr<Thread> msg_thread_ = nullptr;
        std::shared_ptr<Thread> task_thread_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_CONTEXT_H
