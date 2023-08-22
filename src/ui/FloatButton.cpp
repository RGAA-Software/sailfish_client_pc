//
// Created by RGAA on 2023-08-22.
//

#include "FloatButton.h"

#include <QPainter>
#include <QPen>
#include <QBrush>

namespace rgaa {

    FloatButton::FloatButton(QWidget* parent) : QWidget(parent) {
        expand_pixmap_ = QPixmap::fromImage(QImage(":/resources/image/ic_expand.svg"));
        expand_pixmap_ = expand_pixmap_.scaled(expand_pixmap_.width()/2, expand_pixmap_.height()/2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    FloatButton::~FloatButton() {

    }

    void FloatButton::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(Qt::NoPen);
        if (pressed_) {
            painter.setBrush(QBrush(QColor(0x00, 0x00, 0x00, 0x99)));
        }
        else if (enter_) {
            painter.setBrush(QBrush(QColor(0x00, 0x00, 0x00, 0x77)));
        }
        else {
            painter.setBrush(QBrush(QColor(0x00, 0x00, 0x00, 0x55)));
        }

        int w = QWidget::width();
        int h = QWidget::height();
        int radius = QWidget::height()/2;
        painter.drawRoundedRect(0, 0, w, h, radius, radius);

        painter.drawPixmap((w-expand_pixmap_.width())/2, (h-expand_pixmap_.height())/2, expand_pixmap_);
    }

    void FloatButton::enterEvent(QEnterEvent *event) {
        enter_ = true;
        update();
    }

    void FloatButton::leaveEvent(QEvent *event) {
        enter_ = false;
        update();
    }

    void FloatButton::mousePressEvent(QMouseEvent *event) {
        pressed_ = true;
        update();
    }

    void FloatButton::mouseReleaseEvent(QMouseEvent *event) {
        pressed_ = false;
        update();

        if (click_cbk_) {
            click_cbk_();
        }
    }


}