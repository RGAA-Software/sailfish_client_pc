//
// Created by RGAA on 2023-08-22.
//

#include "FloatMenu.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QLabel>
#include <QPainter>

#include "WidgetHelper.h"

namespace rgaa {

    FloatMenuItem::FloatMenuItem(QWidget* parent) : QWidget(parent){
        setFixedSize(50, 50);

        auto item_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(item_layout);
        item_layout->addStretch();

        auto icon_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(icon_layout);
        icon_layout->addStretch();
        auto icon = new QLabel(this);
        icon->setFixedSize(20, 20);
        icon->setStyleSheet("background:#FFFFFF;");
        icon_layout->addWidget(icon);
        icon_layout->addStretch();

        item_layout->addLayout(icon_layout);
        item_layout->addSpacing(5);

        auto text_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(text_layout);
        text_layout->addStretch();
        auto text = new QLabel(this);
        text_layout->addWidget(text);
        item_layout->addLayout(text_layout);
        text->setText("Close");
        text->setStyleSheet("color:#FFFFFF;font-size:12px;");
        text_layout->addStretch();

        item_layout->addStretch();
        setLayout(item_layout);
    }

    FloatMenuItem::~FloatMenuItem() {

    }

    void FloatMenuItem::SetOnClickCallback(std::function<void()>&& cbk) {
        click_cbk_ = std::move(cbk);
    }

    void FloatMenuItem::enterEvent(QEnterEvent *event) {
        enter_ = true;
        this->update();
    }

    void FloatMenuItem::leaveEvent(QEvent *event) {
        enter_ = false;
        this->update();
    }

    void FloatMenuItem::mousePressEvent(QMouseEvent *event) {
        pressed_ = true;
        this->update();
    }

    void FloatMenuItem::mouseReleaseEvent(QMouseEvent *event) {
        pressed_ = false;
        this->update();
    }

    void FloatMenuItem::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing);
        painter.setPen(Qt::NoPen);
        if (pressed_) {
            painter.setBrush(QBrush(QColor(0x66, 0x66, 0x66, 0x55 * transparency_)));
        }
        else if (enter_) {
            painter.setBrush(QBrush(QColor(0x55, 0x55, 0x55, 0x55 * transparency_)));
        }
        else {
            painter.setBrush(QBrush(QColor(0x22, 0x22, 0x22, 0x55 * transparency_)));
        }
        int w = QWidget::width();
        int h = QWidget::height();
        int radius = 5;
        painter.drawRoundedRect(0, 0, w, h, radius, radius);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


    FloatMenu::FloatMenu(QWidget* parent) : QWidget(parent) {
        auto root_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(root_layout);
        root_layout->addStretch();

        auto item_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(item_layout);
        item_layout->addStretch();

        // 1.
        auto item_debug = new FloatMenuItem(this);
        item_layout->addWidget(item_debug);
        item_layout->addStretch();
        menu_items_.push_back(item_debug);

        // 2.
        auto item_clipboard = new FloatMenuItem(this);
        item_layout->addWidget(item_clipboard);
        item_layout->addStretch();
        menu_items_.push_back(item_clipboard);

        // 3.
        auto item_audio = new FloatMenuItem(this);
        item_layout->addWidget(item_audio);
        item_layout->addStretch();
        menu_items_.push_back(item_audio);

        // 4.
        auto item_fullscreen = new FloatMenuItem(this);
        item_layout->addWidget(item_fullscreen);
        item_layout->addStretch();
        menu_items_.push_back(item_fullscreen);

        // 5.
        auto item_close = new FloatMenuItem(this);
        item_layout->addWidget(item_close);
        item_layout->addStretch();
        menu_items_.push_back(item_close);

        root_layout->addLayout(item_layout);
        root_layout->addStretch();
        setLayout(root_layout);
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
        painter.setBrush(QBrush(QColor(0x00, 0x00, 0x00, 0xBB * transparency_)));

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
            for (auto& item : menu_items_) {
                item->UpdateTransparency(this->transparency_);
            }
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
            for (auto& item : menu_items_) {
                item->UpdateTransparency(this->transparency_);
            }
            this->update();
        });
        animation->start();
    }

}