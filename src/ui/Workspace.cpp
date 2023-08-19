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

namespace rgaa {

    Workspace::Workspace(const std::shared_ptr<Context>& ctx, const StreamItem& item) : QMainWindow(nullptr) {
        context_ = ctx;
        stream_item_ = item;
        settings_ = Settings::Instance();
        QString title = "Sailfish client window [ 1 ]";
        setWindowTitle(title);

        sdk_ = std::make_shared<SailfishSDK>(stream_item_);

        auto widget = new QWidget(this);
        auto layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(layout);

        auto render_type = settings_->GetVideoRenderType();
        if (render_type == VideoRenderType::kOpenGL) {
            gl_video_widget_ = new OpenGLVideoWidget(context_, RawImageFormat::kI420, this);
            gl_video_widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
            layout->addWidget(gl_video_widget_);
        }
        else if (render_type == VideoRenderType::kSDL) {
            int default_dup_idx = 0;
            sdl_video_widget_ = new SDLWidgetWrapper(context_, sdk_, default_dup_idx, RawImageFormat::kI420, this);
            video_widgets_.insert(std::make_pair(default_dup_idx, sdl_video_widget_));
            sdl_video_widget_->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
            layout->addWidget(sdl_video_widget_);
        }
        else if (render_type == VideoRenderType::kTestQPixmap) {
            qt_video_label_ = new QLabel(this);
            layout->addWidget(qt_video_label_);
        }

        widget->setLayout(layout);
        setCentralWidget(widget);
        this->resize(settings_->GetWSWidth(), settings_->GetWSHeight());

        close_msg_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeCloseWorkspace, [this] (auto& msg) {
            if (CloseWorkspace()) {
                this->deleteLater();
            }
        }));

    }

    Workspace::~Workspace() {
        LOGI("Workspace exit...");
    }

    void Workspace::Run() {

        sdk_->RegisterConfigCallback([=, this](const std::shared_ptr<NetMessage>& msg) {
            auto config = msg->config();
            int screen_size = config.screen_size();
            LOGI("Screen size : {}", screen_size);
            QMetaObject::invokeMethod(this, [=]() {
                for (int dup_idx = 1; dup_idx < screen_size; dup_idx++) {
                    auto widget = new SDLWidgetWrapper(context_, sdk_, dup_idx, RawImageFormat::kI420, nullptr);
                    widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
                    video_widgets_[dup_idx] = widget;
                    widget->resize(settings_->GetWSWidth(), settings_->GetWSHeight());
                }
            });
        });

        sdk_->RegisterVideoFrameDecodedCallback([=, this](int dup_idx, const std::shared_ptr<RawImage>& image) {
            if (video_widgets_.find(dup_idx) == video_widgets_.end()) {
                return;
            }

            auto video_widget = video_widgets_[dup_idx];

            auto render_type = settings_->GetVideoRenderType();
            if (render_type == VideoRenderType::kOpenGL && gl_video_widget_) {
                gl_video_widget_->RefreshI420Image(image);
                gl_video_widget_->update();
            }

            QMetaObject::invokeMethod(this, [=, this](){
                if (render_type == VideoRenderType::kSDL && sdl_video_widget_) {
                    video_widget->widget_->Init(image->img_width, image->img_height);
                    video_widget->widget_->RefreshI420Image(image);
                }
                else if (render_type == VideoRenderType::kTestQPixmap && qt_video_label_) {
                    QImage to_image((uint8_t*)image->img_buf, image->img_width, image->img_height, QImage::Format_RGB888);
                    auto pixmap = QPixmap::fromImage(to_image);
                    qt_video_label_->setPixmap(pixmap);
                }
            });

        });

        sdk_->Init();

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
        if (close_msg_task_id_ != -1) {
            context_->RemoveMessageTask(close_msg_task_id_);
        }
    }

    void Workspace::closeEvent(QCloseEvent *event) {
        LOGI("Workspace closeEvent...");
        if (CloseWorkspace()) {
            QWidget::closeEvent(event);
        }
        else {
            event->ignore();
        }
    }

    bool Workspace::CloseWorkspace() {

        auto dialog = MessageDialog::Make(context_, tr("Do you want to exit the stream ?"));
        if (dialog->exec() == DialogButton::kCancel) {
            return false;
        }

        Exit();

        for (auto& [k, v] : video_widgets_) {
            v->widget_->Exit();
            if (k != 0) {
                v->deleteLater();
            }
        }
        LOGI("Exit sdl video widget..");

        if (close_cbk_) {
            close_cbk_();
        }

        return true;
    }

    void Workspace::SetOnCloseCallback(OnCloseCallback cbk) {
        close_cbk_ = std::move(cbk);
    }

    StreamItem Workspace::GetStreamItem() {
        return stream_item_;
    }

}