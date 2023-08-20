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

    Workspace::Workspace(const std::shared_ptr<Context>& ctx, const StreamItem& item) : QWidget(nullptr) {
        LOGI("Workspace : {}", (void*)this);
        context_ = ctx;
        stream_item_ = item;
        settings_ = Settings::Instance();
        QString title = "Sailfish client window [ 1 ]";
        setWindowTitle(title);

        sdk_ = std::make_shared<SailfishSDK>(ctx, stream_item_);

        //auto widget = new QWidget(this);
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
            auto video_widget = new SDLWidgetWrapper(context_, sdk_, stream_item_, default_dup_idx, RawImageFormat::kI420, this);
            video_widgets_.insert(std::make_pair(default_dup_idx, video_widget));
            video_widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
            layout->addWidget(video_widget);
        }
        else if (render_type == VideoRenderType::kTestQPixmap) {
            qt_video_label_ = new QLabel(this);
            layout->addWidget(qt_video_label_);
        }

        //widget->setLayout(layout);
        setLayout(layout);
        this->resize(settings_->GetWSWidth(), settings_->GetWSHeight());

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
                    auto widget = new SDLWidgetWrapper(context_, sdk_, stream_item_, dup_idx, RawImageFormat::kI420, nullptr);
                    widget->widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1, std::placeholders::_2));
                    video_widgets_[dup_idx] = widget;
                    widget->resize(settings_->GetWSWidth(), settings_->GetWSHeight());
                    widget->show();

                    connect(widget, &SDLWidgetWrapper::OnCloseEvent, this, [=, this]() {
                        auto msg = ClearWorkspace::Make(stream_item_);
                        context_->SendAppMessage(msg);
                    });
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
                if (render_type == VideoRenderType::kSDL && video_widget) {
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
    }

    void Workspace::closeEvent(QCloseEvent *event) {
        LOGI("Workspace closeEvent...");
        event->ignore();
        CloseWorkspace();
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

        context_->SendAppMessage(CloseWorkspace::Make(stream_item_));

        return true;
    }

    StreamItem Workspace::GetStreamItem() {
        return stream_item_;
    }

}