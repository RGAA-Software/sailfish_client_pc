//
// Created by RGAA on 2023-08-13.
//

#ifndef SAILFISH_CLIENT_PC_APPLICATION_H
#define SAILFISH_CLIENT_PC_APPLICATION_H

#include <QtWidgets/QMainWindow>
#include <QStackedWidget>

#include "sdk/StreamItem.h"

namespace rgaa {

    class Context;
    class Workspace;
    class Thread;
    class AppStreamList;
    class AppMenu;
    class StreamContent;

    class Application : public QMainWindow {
    public:

        explicit Application(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~Application();

    private:

        void StartStreaming(const StreamItem& item);

        void CreateLayout();
        void Init();

        void LoadStyle(const std::string& name);

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::map<std::string, std::shared_ptr<Workspace>> workspaces_;
        AppMenu* app_menu_ = nullptr;
        QStackedWidget* content_widget_ = nullptr;
        StreamContent* stream_content_ = nullptr;

        int clear_ws_task_id_ = -1;
        int close_ws_task_id_ = -1;

    };

}

#endif //SAILFISH_CLIENT_PC_APPLICATION_H
