//
// Created by RGAA on 2023-08-10.
//

#ifndef SAILFISH_CLIENT_PC_WORKSPACE_H
#define SAILFISH_CLIENT_PC_WORKSPACE_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QEvent>

#include "sdk/StreamItem.h"

#define RENDER_OPENGL 1
#define RENDER_SDL !RENDER_OPENGL

namespace rgaa {

    class Settings;
    class OpenGLVideoWidget;
    class SDLVideoWidget;
    class Context;
    class SailfishSDK;
    class NetMessage;
    class SDLWidgetWrapper;
    class WorkspaceCover;
    class SailfishSDK;
    class OpenGLWidgetWrapper;
    class MessageDialog;

    class Workspace : public QWidget {
    public:
        Workspace(const std::shared_ptr<Context>& ctx, const StreamItem& item);
        ~Workspace();

        void Run();
        void Exit();

        void closeEvent(QCloseEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
 
        bool eventFilter(QObject *watched, QEvent *event) override;

        StreamItem GetStreamItem();
        bool CloseWorkspace();

    private:

        void OnMouseKeyboardEventCallback(int dup_idx, const std::shared_ptr<NetMessage>& msg);

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        std::shared_ptr<Context> context_ = nullptr;
        Settings* settings_ = nullptr;

        //OpenGLVideoWidget* gl_video_widget_ = nullptr;
        QLabel* qt_video_label_ = nullptr;

#if RENDER_SDL
        std::map<int, SDLWidgetWrapper*> sdl_video_widgets_;
#endif

#if RENDER_OPENGL
        std::map<int, OpenGLWidgetWrapper*> gl_video_widgets_;
#endif

        StreamItem stream_item_;

        WorkspaceCover* cover_ = nullptr;

        int fullscreen_task_id_ = -1;
        int exit_fullscreen_task_id_ = -1;

        std::shared_ptr<MessageDialog> exit_dialog_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_WORKSPACE_H
