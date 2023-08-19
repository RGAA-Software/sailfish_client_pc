//
// Created by RGAA on 2023/8/19.
//

#include "StreamItemWidget.h"

#include <QPainter>

namespace rgaa {

    StreamItemWidget::StreamItemWidget(int bg_color, QWidget* parent) : QWidget(parent) {
        this->bg_color_ = bg_color;
    }

    StreamItemWidget::~StreamItemWidget() {

    }

    void StreamItemWidget::paintEvent(QPaintEvent *event) {
        //QWidget::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(bg_color_)));
        int radius = 12;
        painter.drawRoundedRect(0, 0, width(), height(), radius, radius);

        if (enter_) {
            int border_width = 2;
            QPen pen(0x386487);
            pen.setWidth(border_width);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(1, 1, width() - border_width, height() - border_width, radius, radius);
        }
    }

    void StreamItemWidget::enterEvent(QEnterEvent *event) {
        enter_ = true;
        update();
    }

    void StreamItemWidget::leaveEvent(QEvent *event) {
        enter_ = false;
        update();
    }

}