//
// Created by RGAA on 2023-08-22.
//

#include "FloatMenu.h"

#include <QPainter>

namespace rgaa {

    FloatMenu::FloatMenu(QWidget* parent) : QWidget(parent) {
        
    }

    FloatMenu::~FloatMenu() {

    }

    void FloatMenu::enterEvent(QEnterEvent *event) {

    }

    void FloatMenu::leaveEvent(QEvent *event) {

    }

    void FloatMenu::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(0x00, 0x00, 0x00, 0x77)));

        int w = QWidget::width();
        int h = QWidget::height();
        int radius = 11;
        painter.drawRoundedRect(0, 0, w, h, radius, radius);
    }

}