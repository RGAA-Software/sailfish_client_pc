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
#include "Context.h"
#include "AppMessage.h"
#include "rgaa_common/RMessageQueue.h"

namespace rgaa {

    FloatMenuItem::FloatMenuItem(const QString& name, const QString& normal_icon_path, const QString& expand_icon_path, QWidget* parent) : QWidget(parent){
        setFixedSize(60, 50);

        if (!normal_icon_path.isEmpty()) {
            normal_icon_ = QPixmap::fromImage(QImage(normal_icon_path));
            normal_icon_ = normal_icon_.scaled(normal_icon_.width()/2.2f, normal_icon_.height()/2.2f, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        if (!expand_icon_path.isEmpty()) {
            expand_icon_ = QPixmap::fromImage(QImage(expand_icon_path));
            expand_icon_ = expand_icon_.scaled(expand_icon_.width()/2.2f, expand_icon_.height()/2.2f, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        auto item_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(item_layout);
        item_layout->addStretch();

        auto icon_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(icon_layout);
        icon_layout->addStretch();
        auto icon = new QLabel(this);
        icon_ = icon;
        icon->setFixedSize(25, 25);
        //icon->setStyleSheet("background:#FFFFFF;");
        if (!normal_icon_.isNull()) {
            icon_->setPixmap(normal_icon_);
        }
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
        text->setText(name);
        text->setStyleSheet("color:#FFFFFF;font-size:11px;");
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
        if (click_cbk_) {
            click_cbk_();
        }
    }

    void FloatMenuItem::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing);
        painter.setPen(Qt::NoPen);
        if (pressed_) {
            painter.setBrush(QBrush(QColor(0x66, 0x66, 0x66, 0x55 * transparency_)));
        }
        else if (enter_ || expand_) {
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

    void FloatMenuItem::UpdateTransparency(float v) {
        transparency_ = v;
    }

    void FloatMenuItem::SetExpand(bool st) {
        this->expand_ = st;
        if (this->expand_ && !expand_icon_.isNull()) {
            icon_->setPixmap(expand_icon_);
        }
        else if (!this->expand_ && !normal_icon_.isNull()) {
            icon_->setPixmap(normal_icon_);
        }
    }

    bool FloatMenuItem::GetExpand() {
        return expand_;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


    FloatMenu::FloatMenu(const std::shared_ptr<Context>& ctx, const StreamItem& item, QWidget* parent) : QWidget(parent) {
        this->context_ = ctx;
        this->item_ = item;
        auto root_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(root_layout);
        root_layout->addStretch();

        auto item_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(item_layout);
        item_layout->addStretch();

        // 1.
        auto item_debug = new FloatMenuItem(tr("Debug"), ":/resources/image/ic_debug_off.svg", ":/resources/image/ic_debug_on.svg", this);
        item_layout->addWidget(item_debug);
        item_layout->addStretch();
        menu_items_.push_back(item_debug);
        item_debug->SetOnClickCallback([=, this]() {
            auto expand = item_debug->GetExpand();
            auto to_status = !expand;
            item_debug->SetExpand(to_status);
            if (debug_status_cbk_) {
                debug_status_cbk_(to_status);
            }
        });

        // 2.
        auto item_clipboard = new FloatMenuItem(tr("Clipboard"), ":/resources/image/ic_clipboard_off.svg", ":/resources/image/ic_clipboard_on.svg", this);
        item_layout->addWidget(item_clipboard);
        item_layout->addStretch();
        menu_items_.push_back(item_clipboard);
        item_clipboard->SetOnClickCallback([=, this] () {
            auto expand = item_clipboard->GetExpand();
            auto to_status = !expand;
            item_clipboard->SetExpand(to_status);
            context_->SendAppMessage(ClipboardStatusMessage::Make(to_status));
        });

        // 3.
        auto item_audio = new FloatMenuItem(tr("Audio"), ":/resources/image/ic_audio_off.svg", ":/resources/image/ic_audio_on.svg", this);
        item_layout->addWidget(item_audio);
        item_layout->addStretch();
        menu_items_.push_back(item_audio);
        item_audio->SetOnClickCallback([=, this] () {
            auto expand = item_audio->GetExpand();
            auto to_status = !expand;
            item_audio->SetExpand(to_status);
            context_->SendAppMessage(AudioStatusMessage::Make(to_status));
        });

        // 4.
        auto item_fullscreen = new FloatMenuItem(tr("Fullscreen"), ":/resources/image/ic_fullscreen.svg", ":/resources/image/ic_exit_fullscreen.svg", this);
        item_layout->addWidget(item_fullscreen);
        item_layout->addStretch();
        menu_items_.push_back(item_fullscreen);
        item_fullscreen->SetExpand(false);
        item_fullscreen->SetOnClickCallback([=, this] () {
            auto expand = item_fullscreen->GetExpand();
            auto to_status = !expand;
            item_fullscreen->SetExpand(to_status);

            if (to_status) {
                context_->SendAppMessage(FullscreenMessage::Make());
            }
            else {
                context_->SendAppMessage(ExitFullscreenMessage::Make());
            }

        });

        // 5.
        auto item_close = new FloatMenuItem(tr("Close"), ":/resources/image/ic_close.svg", "", this);
        item_layout->addWidget(item_close);
        item_layout->addStretch();
        menu_items_.push_back(item_close);
        item_close->SetOnClickCallback([=, this]() {
            context_->SendAppMessage(ClearWorkspace::Make(item_));
        });

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
        QWidget::paintEvent(event);

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
        animation->setDuration(150);
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

    void FloatMenu::Show() {
        this->transparency_ = 1.0;
        show();
    }

    void FloatMenu::Hide() {
        this->transparency_ = 0;
        hide();
    }

    void FloatMenu::HideWithAnim(std::function<void()>&& finished_task) {
        auto animation = new QPropertyAnimation();
        animation->setDuration(150);
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
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start();
    }

    void FloatMenu::SetOnDebugStatusCallback(std::function<void(bool)>&& cbk) {
        debug_status_cbk_ = std::move(cbk);
    }

}