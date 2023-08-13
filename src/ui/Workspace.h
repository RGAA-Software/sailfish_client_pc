//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_WORKSPACE_H
#define SAILFISH_CLIENT_PC_WORKSPACE_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

namespace rgaa {

    class Settings;
    class OpenGLVideoWidget;
    class Context;
    class SailfishSDK;

    class Workspace : public QMainWindow {
    public:
        Workspace(const std::shared_ptr<Context>& ctx);
        ~Workspace();

        void Run();

        OpenGLVideoWidget* GetVideoWidget();

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        std::shared_ptr<Context> context_ = nullptr;
        Settings* settings_ = nullptr;
        OpenGLVideoWidget* video_widget_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_WORKSPACE_H
