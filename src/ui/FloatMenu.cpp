//
// Created by RGAA on 2023-08-22.
//

#include "FloatMenu.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>

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
        painter.setBrush(QBrush(QColor(0x00, 0x00, 0x00, 0x77 * transparency_)));

        int w = QWidget::width();
        int h = QWidget::height();
        int radius = 11;
        painter.drawRoundedRect(0, 0, w, h, radius, radius);
    }

    void FloatMenu::ShowWithAnim() {
        auto animation = new QPropertyAnimation();
        animation->setDuration(350);
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
        connect(animation, &QPropertyAnimation::finished, this, [=]() {
            delete animation;
        });
        connect(animation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
            this->transparency_ = value.toFloat();
            this->update();
        });
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start();
        show();
    }

    void FloatMenu::Hide() {
        this->transparency_ = 0;
        hide();
    }

    void FloatMenu::HideWithAnim(std::function<void()>&& finished_task) {
        auto animation = new QPropertyAnimation();
        animation->setDuration(250);
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);
        connect(animation, &QPropertyAnimation::finished, this, [=]() {
            if (finished_task) {
                finished_task();
            }
            this->hide();
            delete animation;
        });
        connect(animation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
            this->transparency_ = value.toFloat();
            this->update();
        });
        animation->start();
    }

}