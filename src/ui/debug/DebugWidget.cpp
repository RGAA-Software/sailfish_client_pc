//
// Created by RGAA on 2023/8/24.
//

#include "DebugWidget.h"

#include "Context.h"
#include "sdk/Statistics.h"
#include "sdk/SailfishSDK.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QLabel>

#include "ui/WidgetHelper.h"
#include "LineChart.h"

namespace rgaa {

    DebugWidget::DebugWidget(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, QWidget* parent) : QWidget(parent) {
        context_ = ctx;
        sdk_ = sdk;
        statistics_ = sdk_->GetStatistics();

        auto root_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(root_layout);
        root_layout->addSpacing(20);
        // left
        {
            auto layout = new QVBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(20);
            decode_time_chart_ = new LineChart(tr("Video decode duration(Client)"), this);
            decode_time_chart_->setFixedSize(450, 120);
            layout->addWidget(decode_time_chart_);

            layout->addSpacing(30);

            video_frame_time_chart_ = new LineChart(tr("Video frame gaps"), this);
            video_frame_time_chart_->setFixedSize(450, 120);
            layout->addWidget(video_frame_time_chart_);

            layout->addSpacing(30);

            encode_time_chart_ = new LineChart(tr("[Capture + Encode] duration(Server)"), this);
            encode_time_chart_->setFixedSize(450, 120);
            layout->addWidget(encode_time_chart_);

            layout->addSpacing(30);

            network_time_chart_ = new LineChart(tr("Server -> Client -> Server duration"), this);
            network_time_chart_->setFixedSize(450, 120);
            layout->addWidget(network_time_chart_);

            layout->addStretch();
            root_layout->addLayout(layout);
        }
        // right
        {
            auto right_layout = new QVBoxLayout();
            WidgetHelper::ClearMargin(right_layout);
            right_layout->addSpacing(20);
            root_layout->addLayout(right_layout);

            right_grid_ = new QGridLayout();
            right_layout->addLayout(right_grid_);
            WidgetHelper::ClearMargin(right_grid_);
            root_layout->addLayout(right_grid_);

            auto size = QSize(180, 30);
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Recv video FPS"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 0, 0);

                auto value = new QLabel(this);
                recv_fps_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 0, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Recv video bytes(MB)"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 1, 0);

                auto value = new QLabel(this);
                recv_video_bytes_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 1, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Video encode format"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 2, 0);

                auto value = new QLabel(this);
                video_encode_format_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 2, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Video width"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 3, 0);

                auto value = new QLabel(this);
                video_width_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 3, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Video height"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 4, 0);

                auto value = new QLabel(this);
                video_height_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 4, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Streaming time"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 5, 0);

                auto value = new QLabel(this);
                streaming_time_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 5, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Audio bytes"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 6, 0);

                auto value = new QLabel(this);
                audio_bytes_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 6, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Audio samples"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 7, 0);

                auto value = new QLabel(this);
                audio_samples_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 7, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Audio channels"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 8, 0);

                auto value = new QLabel(this);
                audio_channel_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 8, 1);
            }
            {
                auto label = new QLabel(this);
                label->setStyleSheet("color:#ffffff;");
                label->setText(tr("Audio format"));
                label->setFixedSize(size);
                right_grid_->addWidget(label, 9, 0);

                auto value = new QLabel(this);
                audio_format_ = value;
                value->setStyleSheet("color:#ffffff;");
                value->setFixedSize(size);
                value->setText(tr("60"));
                right_grid_->addWidget(value, 9, 1);
            }
            right_layout->addStretch();
        }

        root_layout->addSpacing(20);
        setLayout(root_layout);

        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, [this]() {
            this->UpdateStatistics();
        });
        timer_->start(33);
    }

    DebugWidget::~DebugWidget() {
        timer_->stop();
    }

    void DebugWidget::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(0x22, 0x22, 0x22, 0xBB)));

        int w = this->width();
        int h = this->height();
        int radius = 11;
        painter.drawRoundedRect(0, 0, w, h, radius, radius);
    }

    void DebugWidget::UpdateStatistics() {
        decode_time_chart_->UpdateData(statistics_->video_decode_times);
        video_frame_time_chart_->UpdateData(statistics_->video_recv_diff_times);
        if (statistics_->encode_times_.find(0) != statistics_->encode_times_.end()) {
            encode_time_chart_->UpdateData(statistics_->encode_times_[0]);
        }
        network_time_chart_->UpdateData(statistics_->network_times_);

        recv_fps_->setText(QString::number(statistics_->video_recv_fps));
        recv_video_bytes_->setText(QString::number(statistics_->FormatVideoFrameMB()) + " MB");
        video_encode_format_->setText(statistics_->video_encode_format.c_str());
        video_width_->setText(QString::number(statistics_->video_width));
        video_height_->setText(QString::number(statistics_->video_height));
        streaming_time_->setText(statistics_->FormatStreamingTime().c_str());

        audio_bytes_->setText(statistics_->FormatAudioFrameMB().c_str());
        audio_channel_->setText(QString::number(statistics_->audio_channel_));
        audio_samples_->setText(QString::number(statistics_->audio_samples_));
        audio_format_->setText(statistics_->audio_format_.c_str());

    }

}