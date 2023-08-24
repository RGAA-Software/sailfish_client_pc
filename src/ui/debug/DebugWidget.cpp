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
            decode_time_chart_ = new LineChart(tr("Video decode time(Client)"), this);
            decode_time_chart_->setFixedSize(450, 120);
            layout->addWidget(decode_time_chart_);

            layout->addSpacing(30);

            video_frame_time_chart_ = new LineChart(tr("Video receive gaps(Client)"), this);
            video_frame_time_chart_->setFixedSize(450, 120);
            layout->addWidget(video_frame_time_chart_);

            layout->addSpacing(30);

            encode_time_chart_ = new LineChart(tr("Video encode time(Server)"), this);
            encode_time_chart_->setFixedSize(450, 120);
            layout->addWidget(encode_time_chart_);

            layout->addStretch();
            root_layout->addLayout(layout);
        }
        // right
        {
            root_layout->addStretch();
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
    }

}