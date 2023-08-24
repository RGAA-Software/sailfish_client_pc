//
// Created by RGAA on 2023/8/24.
//

#include "DebugWidget.h"

#include "Context.h"
#include "sdk/Statistics.h"

#include <QPainter>
#include <QPen>
#include <QBrush>

#include "ui/WidgetHelper.h"
#include "LineChart.h"

namespace rgaa {

    DebugWidget::DebugWidget(const std::shared_ptr<Context>& ctx, QWidget* parent) : QWidget(parent) {
        context_ = ctx;
        statistics_ = Statistics::Instance();

        auto root_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(root_layout);

        // left
        {
            auto layout = new QVBoxLayout();
            WidgetHelper::ClearMargin(layout);
            decode_time_chart_ = new LineChart(this);
            decode_time_chart_->setFixedSize(500, 200);
            layout->addWidget(decode_time_chart_);

            root_layout->addLayout(layout);
        }
        // right
        {
            root_layout->addStretch();
        }
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
    }

}