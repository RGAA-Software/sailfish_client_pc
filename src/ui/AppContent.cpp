//
// Created by RGAA on 2023/8/16.
//

#include "AppContent.h"

#include "settings/Settings.h"

#include <QPainter>
#include <QPen>
#include <QBrush>

namespace rgaa {

    AppContent::AppContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : RoundRectWidget(0xEAF7FF, 10, parent) {
        context_ = ctx;
        settings_ = Settings::Instance();
    }

    AppContent::~AppContent() {

    }

    void AppContent::paintEvent(QPaintEvent *event) {
        RoundRectWidget::paintEvent(event);

//        QPainter painter(this);
//        painter.setPen(Qt::NoPen);
//        painter.setBrush(QBrush(QColor(0xEAF7FF)));
//        painter.drawRect(0, 0, QWidget::width(), QWidget::height());
    }

    void AppContent::OnContentShow() {

    }

    void AppContent::OnContentHide() {

    }

}