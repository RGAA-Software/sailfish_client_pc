//
// Created by RGAA on 2023-08-10.
//

#include "Workspace.h"

#include "settings/Settings.h"
#include "OpenGLVideoWidget.h"
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
        video_widget_ = new OpenGLVideoWidget(context_, RawImageFormat::kI420, this);
        layout->addWidget(video_widget_);

//        video_widget_->hide();
        auto test_label = new QLabel();
        layout->addWidget(test_label);
        test_label->hide();

        widget->setLayout(layout);

        setCentralWidget(widget);

        sdk_ = std::make_shared<SailfishSDK>();
        sdk_->RegisterVideoFrameDecodedCallback([=, this](const std::shared_ptr<RawImage>& image) {
            video_widget_->RefreshI420Image(image);
//            video_widget_->RefreshRGBImage(image);
            video_widget_->update();

//            LOGI("RawImage format : {}", (int)image->img_format);
//            QMetaObject::invokeMethod(this, [=](){
//                QImage to_image((uint8_t*)image->img_buf, image->img_width, image->img_height, QImage::Format_RGB888);
//                auto pixmap = QPixmap::fromImage(to_image);
//                test_label->setPixmap(pixmap);
//            });
        });

        sdk_->Init();
    }

    Workspace::~Workspace() {

    }

    void Workspace::Run() {
        show();
    }

    OpenGLVideoWidget* Workspace::GetVideoWidget() {
        return video_widget_;
    }

}