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

namespace rgaa {

    Workspace::Workspace(const std::shared_ptr<Context>& ctx) : QMainWindow(nullptr) {
        context_ = ctx;
        settings_ = Settings::Instance();
        resize(settings_->GetWSWidth(), settings_->GetWSHeight());

        auto widget = new QWidget(this);
        auto layout = new QVBoxLayout();

        auto render_type = settings_->GetVideoRenderType();
        if (render_type == VideoRenderType::kOpenGL) {
            gl_video_widget_ = new OpenGLVideoWidget(context_, RawImageFormat::kI420, this);
            layout->addWidget(gl_video_widget_);
        }
        else if (render_type == VideoRenderType::kSDL) {
            sdl_video_widget_ = new SDLVideoWidget(context_, RawImageFormat::kI420, this);
            sdl_video_widget_->RegisterMouseKeyboardEventCallback(std::bind(&Workspace::OnMouseKeyboardEventCallback, this, std::placeholders::_1));
            layout->addWidget(sdl_video_widget_);
        }
        else if (render_type == VideoRenderType::kTestQPixmap) {
            qt_video_label_ = new QLabel(this);
            layout->addWidget(qt_video_label_);
        }

        widget->setLayout(layout);

        setCentralWidget(widget);
    }

    Workspace::~Workspace() {

    }

    void Workspace::Run() {

        sdk_ = std::make_shared<SailfishSDK>();
        sdk_->RegisterVideoFrameDecodedCallback([=, this](const std::shared_ptr<RawImage>& image) {
            auto render_type = settings_->GetVideoRenderType();
            if (render_type == VideoRenderType::kOpenGL && gl_video_widget_) {
                gl_video_widget_->RefreshI420Image(image);
                gl_video_widget_->update();
            }

            QMetaObject::invokeMethod(this, [=, this](){
                if (render_type == VideoRenderType::kSDL && sdl_video_widget_) {
                    sdl_video_widget_->Init(image->img_width, image->img_height);
                    sdl_video_widget_->RefreshI420Image(image);
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

    void Workspace::OnMouseKeyboardEventCallback(const std::shared_ptr<NetMessage>& msg) {
        if (sdk_) {
            sdk_->PostNetMessage(msg);
        }
    }

}