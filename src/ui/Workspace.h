//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_WORKSPACE_H
#define SAILFISH_CLIENT_PC_WORKSPACE_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>

namespace rgaa {

    class Settings;
    class OpenGLVideoWidget;
    class SDLVideoWidget;
    class Context;
    class SailfishSDK;
    class NetMessage;

    class Workspace : public QMainWindow {
    public:
        Workspace(const std::shared_ptr<Context>& ctx);
        ~Workspace();

        void Run();

    private:

        void OnMouseKeyboardEventCallback(const std::shared_ptr<NetMessage>& msg);

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        std::shared_ptr<Context> context_ = nullptr;
        Settings* settings_ = nullptr;

        OpenGLVideoWidget* gl_video_widget_ = nullptr;
        SDLVideoWidget* sdl_video_widget_ = nullptr;
        QLabel* qt_video_label_ = nullptr;
    };

}

#endif //SAILFISH_CLIENT_PC_WORKSPACE_H
