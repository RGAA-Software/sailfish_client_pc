//
// Created by RGAA on 2023-08-10.
//

#include "Workspace.h"

#include "settings/Settings.h"
#include "OpenGLVideoWidget.h"
#include "SDLVideoWidget.h"
#include "Context.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "sdk/SailfishSDK.h"
#include "sdk/RawImage.h"
#include "rgaa_common/RLog.h"
#include "messages.pb.h"
#include "WidgetHelper.h"
#include "MessageDialog.h"
#include "rgaa_common/RMessageQueue.h"
#include "AppMessage.h"
#include "sdk/MessageParser.h"
#include "WorkspaceCover.h"

namespace rgaa {

    Workspace::Workspace(const std::shared_ptr<Context>& ctx, const StreamItem& item) : QWidget(nullptr) {
        context_ = ctx;
        stream_item_ = item;
        settings_ = Settings::Instance();
        QString title = "Sailfish client window [ 1 ]";
        setWindowTitle(title);

        sdk_ = std::make_shared<SailfishSDK>(ctx, stream_item_);

        auto layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(layout);
        int default_dup_idx = 0;

        auto render_type = settings_->GetVideoRenderType();
#if RENDER_OPENGL
        if (render_type == VideoRenderType::kOpenGL) {
            auto video_widget = new OpenGLWidgetWrapper(context_, sdk_, stream_item_, default_dup_idx, RawImageFormat::kI420, this);
            gl_video_widgets_.insert({default_dup_idx, video_widget});
            video_widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
            layout->addWidget(video_widget);
        }
#endif
#if RENDER_SDL
        if (render_type == VideoRenderType::kSDL) {
            auto video_widget = new SDLWidgetWrapper(context_, sdk_, stream_item_, default_dup_idx, RawImageFormat::kI420, this);
            sdl_video_widgets_.insert(std::make_pair(default_dup_idx, video_widget));
            video_widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
            layout->addWidget(video_widget);
        }
#endif
        if (render_type == VideoRenderType::kTestQPixmap) {
            qt_video_label_ = new QLabel(this);
            layout->addWidget(qt_video_label_);
        }

        setLayout(layout);
        this->resize(settings_->GetWSWidth(), settings_->GetWSHeight());

        cover_ = new WorkspaceCover(context_, sdk_, stream_item_, this);
        cover_->show();
        installEventFilter(this);

        // messages //
        fullscreen_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeFullscreen, [=, this](auto& msg) {
            context_->PostUITask([=, this]() {
                showFullScreen();
#if RENDER_SDL
                for (const auto& [idx, w] : sdl_video_widgets_) {
#endif
#if RENDER_OPENGL
                for (const auto& [idx, w] : gl_video_widgets_) {
#endif
                    if (idx == 0) {
                        continue;
                    }
                    w->showFullScreen();
                }
            });
        }));

        exit_fullscreen_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeExitFullscreen, [=, this](auto& msg) {
            context_->PostUITask([=, this]() {
                showNormal();
#if RENDER_SDL
                for (const auto& [idx, w] : sdl_video_widgets_) {
#endif
#if RENDER_OPENGL
                for (const auto& [idx, w] : gl_video_widgets_) {
#endif
                    if (idx == 0) {
                        continue;
                    }
                    w->showNormal();
                }
            });
        }));
        // messages //
    }

    Workspace::~Workspace() {
        LOGI("Workspace exit... {}", (void*)this);
    }

    void Workspace::Run() {

        sdk_->RegisterConfigCallback([=, this](const std::shared_ptr<NetMessage>& msg) {
            auto config = msg->config();
            int screen_size = config.screen_size();
            LOGI("Screen size : {}", screen_size);
            QMetaObject::invokeMethod(this, [=]() {
                for (int dup_idx = 1; dup_idx < screen_size; dup_idx++) {
#if RENDER_SDL
                    auto widget = new SDLWidgetWrapper(context_, sdk_, stream_item_, dup_idx, RawImageFormat::kI420, nullptr);
                    widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
                    sdl_video_widgets_[dup_idx] = widget;
                    widget->resize(settings_->GetWSWidth(), settings_->GetWSHeight());
                    widget->show();

                    connect(widget, &SDLWidgetWrapper::OnCloseEvent, this, [=, this]() {
                        auto msg = ClearWorkspace::Make(stream_item_);
                        context_->SendAppMessage(msg);
                    });
#endif
#if RENDER_OPENGL
                    auto widget = new OpenGLWidgetWrapper(context_, sdk_, stream_item_, dup_idx, RawImageFormat::kI420, nullptr);
                    widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
                    gl_video_widgets_[dup_idx] = widget;
                    widget->resize(settings_->GetWSWidth(), settings_->GetWSHeight());
                    widget->show();

                    connect(widget, &OpenGLWidgetWrapper::OnCloseEvent, this, [=, this]() {
                        auto msg = ClearWorkspace::Make(stream_item_);
                        context_->SendAppMessage(msg);
                    });
#endif
                }
            });
        });

        sdk_->RegisterVideoFrameDecodedCallback([=, this](int dup_idx, const std::shared_ptr<RawImage>& image) {
#if RENDER_OPENGL
            auto render_type = settings_->GetVideoRenderType();
            if (render_type == VideoRenderType::kOpenGL) {

                if (gl_video_widgets_.find(dup_idx) == gl_video_widgets_.end()) {
                    return;
                }
                auto video_widget = gl_video_widgets_[dup_idx];
                if (video_widget) {
                    //video_widget->widget_->Init(image->img_width, image->img_height);
                    video_widget->widget_->RefreshI420Image(image);
                }

                return;
            }
#endif

            QMetaObject::invokeMethod(this, [=, this](){
#if RENDER_SDL
                if (sdl_video_widgets_.find(dup_idx) == sdl_video_widgets_.end()) {
                    return;
                }
                auto video_widget = sdl_video_widgets_[dup_idx];
                if (render_type == VideoRenderType::kSDL && video_widget) {
                    video_widget->widget_->Init(image->img_width, image->img_height);
                    video_widget->widget_->RefreshI420Image(image);
                }
#endif
                if (render_type == VideoRenderType::kTestQPixmap && qt_video_label_) {
                    QImage to_image((uint8_t*)image->img_buf, image->img_width, image->img_height, QImage::Format_RGB888);
                    auto pixmap = QPixmap::fromImage(to_image);
                    qt_video_label_->setPixmap(pixmap);
                }
            });

        });

        sdk_->Init();

        sdk_->GetMsgParser()->SetOnCursorCallback([=, this](int dup_idx, int x, int y, int hpx, int hpy, const RawImagePtr& image) {
            auto render_type = settings_->GetVideoRenderType();

#if RENDER_SDL
            if (sdl_video_widgets_.find(dup_idx) == sdl_video_widgets_.end()) {
                return;
            }
            QMetaObject::invokeMethod(this, [=, this](){
                auto video_widget = sdl_video_widgets_[dup_idx];
                video_widget->widget_->RefreshCursor(x, y, hpx, hpy, image);
            });
#endif
#if RENDER_OPENGL
            if (gl_video_widgets_.find(dup_idx) == gl_video_widgets_.end()) {
                return;
            }
            auto video_widget = gl_video_widgets_[dup_idx];
            video_widget->widget_->RefreshCursor(x, y, hpx, hpy, image);
#endif

        });

        show();
    }

    void Workspace::OnMouseKeyboardEventCallback(int dup_idx, const std::shared_ptr<NetMessage>& msg) {
        if (sdk_) {
            sdk_->PostNetMessage(msg);
        }
    }

    void Workspace::Exit() {
        if (sdk_) {
            sdk_->Exit();
        }
        context_->RemoveMessageTask(fullscreen_task_id_);
        context_->RemoveMessageTask(exit_fullscreen_task_id_);
    }

    void Workspace::closeEvent(QCloseEvent *event) {
        LOGI("Workspace closeEvent...");
        event->ignore();
        CloseWorkspace();
    }

    void Workspace::resizeEvent(QResizeEvent *event) {
        QWidget::resizeEvent(event);
        // use Widget size!!! QResizeEvent give a different size of the widget.
        //cover_->setFixedSize(this->size());
        if (cover_) {
            cover_->OnWindowResize();
        }
    }

    bool Workspace::eventFilter(QObject *watched, QEvent *event) {
        if (watched == this) {
            if (event->type() == QEvent::Move) {
                if (cover_) {
                    cover_->OnWindowMove();
                }
            }
            else if (event->type() == QEvent::NonClientAreaMouseButtonPress) {
                context_->SendAppMessage(MousePressedMessage::Make());
            }
        }

        return QWidget::eventFilter(watched, event);
    }

    bool Workspace::CloseWorkspace() {

        auto dialog = MessageDialog::Make(context_, tr("Do you want to exit the stream ?"));
        if (dialog->exec() == DialogButton::kCancel) {
            return false;
        }

        Exit();
#if RENDER_SDL
        for (auto& [k, v] : sdl_video_widgets_) {
            v->widget_->Exit();
            if (k != 0) {
                v->deleteLater();
            }
        }
#endif
#if RENDER_OPENGL
        for (auto& [k, v] : gl_video_widgets_) {
            v->widget_->Exit();
            if (k != 0) {
                v->deleteLater();
            }
        }
#endif

        context_->SendAppMessage(CloseWorkspace::Make(stream_item_));

        return true;
    }

    StreamItem Workspace::GetStreamItem() {
        return stream_item_;
    }

}