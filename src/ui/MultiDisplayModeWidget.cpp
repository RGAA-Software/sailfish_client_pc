//
// Created by RGAA on 2023-09-02.
//

#include "MultiDisplayModeWidget.h"

namespace rgaa {

    MultiDisplayModeWidget::MultiDisplayModeWidget(MultiDisplayMode mode, QWidget* parent) : QWidget(parent) {
        display_mode_ = mode;
    }

    MultiDisplayModeWidget::~MultiDisplayModeWidget() {

    }

    void MultiDisplayModeWidget::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        int radius = 10;
        if (enter_) {
            painter.setBrush(QColor(0xC0DCF2));
        }
        else {
            painter.setBrush(QColor(0xffffff));
        }
        painter.drawRoundedRect(0, 0, this->width(), this->height(), radius, radius);

        QPen pen(QColor(0x386487));
        pen.setWidth(1);
        painter.setPen(pen);

        int solid_color = 0x6894b7;

        auto brush = QBrush(solid_color);
        brush.setStyle(Qt::BrushStyle::BDiagPattern);
        painter.setBrush(brush);

        //painter.setBrush(Qt::NoBrush);

        if (display_mode_ == MultiDisplayMode::kSeparated) {
            int x_border_gap = 4;
            int y_border_gap = 10;
            int width = (this->width() - x_border_gap*3)/2;
            painter.drawRoundedRect(x_border_gap, y_border_gap, width, this->height()-y_border_gap*2, radius, radius);
            painter.drawRoundedRect(x_border_gap*2 + width, y_border_gap, width, this->height()-y_border_gap*2, radius, radius);

            painter.setBrush(QBrush(solid_color));
            painter.drawEllipse(QPoint(this->width() - 2*x_border_gap - width - 16, y_border_gap + 16), 8, 8);
            painter.drawEllipse(QPoint(this->width() - x_border_gap - 16, y_border_gap + 16), 8, 8);
        }
        else if (display_mode_ == MultiDisplayMode::kCombined) {
            int x_border_gap = 13;
            int y_border_gap = 10;
            painter.drawRoundedRect(x_border_gap, y_border_gap, this->width()-x_border_gap*2, this->height() - y_border_gap*2, radius, radius);
            painter.setBrush(QBrush(solid_color));
            painter.drawEllipse(QPoint(this->width() - x_border_gap - 16, y_border_gap + 16), 8, 8);
        }
    }

    void MultiDisplayModeWidget::enterEvent(QEnterEvent *event) {
        enter_ = true;
        update();
    }

    void MultiDisplayModeWidget::leaveEvent(QEvent *event) {
        enter_ = false;
        update();
    }

    void MultiDisplayModeWidget::mouseReleaseEvent(QMouseEvent *event) {
        if (click_cbk_) {
            click_cbk_();
        }
    }


}