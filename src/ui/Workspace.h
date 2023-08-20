//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_WORKSPACE_H
#define SAILFISH_CLIENT_PC_WORKSPACE_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>

#include "sdk/StreamItem.h"

namespace rgaa {

    class Settings;
    class OpenGLVideoWidget;
    class SDLVideoWidget;
    class Context;
    class SailfishSDK;
    class NetMessage;
    class SDLWidgetWrapper;

    class Workspace : public QWidget {
    public:
        Workspace(const std::shared_ptr<Context>& ctx, const StreamItem& item);
        ~Workspace();

        void Run();
        void Exit();

        void closeEvent(QCloseEvent *event) override;

        StreamItem GetStreamItem();
        bool CloseWorkspace();

    private:

        void OnMouseKeyboardEventCallback(int dup_idx, const std::shared_ptr<NetMessage>& msg);

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        std::shared_ptr<Context> context_ = nullptr;
        Settings* settings_ = nullptr;

        OpenGLVideoWidget* gl_video_widget_ = nullptr;
        QLabel* qt_video_label_ = nullptr;

        std::map<int, SDLWidgetWrapper*> video_widgets_;

        StreamItem stream_item_;

    };

}

#endif //SAILFISH_CLIENT_PC_WORKSPACE_H
