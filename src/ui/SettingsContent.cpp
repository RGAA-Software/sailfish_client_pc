//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

#include "widgets/SwitchButton.h"
#include "WidgetHelper.h"
#include "MultiDisplayModeWidget.h"
#include "Settings.h"

namespace rgaa {

    SettingsContent::SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        settings_ = Settings::Instance();

        auto root_layout = new QVBoxLayout();
        root_layout->addSpacing(20);
        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(50);

            auto label = new QLabel(this);
            label->setFixedSize(330, 40);
            label->setText(tr("fix the menu at top"));
            label->setStyleSheet("font-size:12pt;");
            layout->addWidget(label);

            auto switch_btn = new SwitchButton(this);
            switch_btn->SetStatus(settings_->IsFixAtTop());
            switch_btn->setFixedSize(55, 25);
            switch_btn->SetClickCallback([this](bool selected) {
                settings_->SetFixAtTop(selected);
            });
            layout->addSpacing(30);
            layout->addWidget(switch_btn);
            layout->addStretch();

            root_layout->addLayout(layout);
        }
        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(50);

            auto label = new QLabel(this);
            label->setFixedSize(330, 40);
            label->setText(tr("audio enabled"));
            label->setStyleSheet("font-size:12pt;");
            layout->addWidget(label);

            auto switch_btn = new SwitchButton(this);
            switch_btn->SetStatus(settings_->IsAudioEnabled());
            switch_btn->setFixedSize(55, 25);
            switch_btn->SetClickCallback([this](bool selected) {
                settings_->SetAudioEnabled(selected);
            });
            layout->addSpacing(30);
            layout->addWidget(switch_btn);
            layout->addStretch();

            root_layout->addLayout(layout);
        }
        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(50);

            auto label = new QLabel(this);
            label->setFixedSize(330, 40);
            label->setText(tr("clipboard enabled"));
            label->setStyleSheet("font-size:12pt;");
            layout->addWidget(label);

            auto switch_btn = new SwitchButton(this);
            switch_btn->SetStatus(settings_->IsClipboardEnabled());
            switch_btn->setFixedSize(55, 25);
            switch_btn->SetClickCallback([=](bool selected) {
                settings_->SetClipboardEnabled(selected);
            });
            layout->addSpacing(30);
            layout->addWidget(switch_btn);
            layout->addStretch();

            root_layout->addLayout(layout);
        }
        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(50);

            auto label = new QLabel(this);
            label->setFixedSize(330, 40);
            label->setText(tr("multiple monitors display mode"));
            label->setStyleSheet("font-size:12pt;");
            layout->addWidget(label);
            layout->addStretch();
            root_layout->addLayout(layout);
        }

        {
            root_layout->addSpacing(15);
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(50);



            // separated
            separated_ = new MultiDisplayModeWidget(MultiDisplayMode::kSeparated, this);
            separated_->SetSelected(settings_->GetMultiDisplayMode() == MultiDisplayMode::kSeparated);
            separated_->setFixedSize(250, 150);
            separated_->SetOnClickCallback([=]() {
                combined_->SetSelected(false);
                settings_->SetMultiDisplayMode(MultiDisplayMode::kSeparated);
            });
            layout->addWidget(separated_);

            layout->addSpacing(20);

            combined_ = new MultiDisplayModeWidget(MultiDisplayMode::kCombined, this);
            combined_->SetSelected(settings_->GetMultiDisplayMode() == MultiDisplayMode::kCombined);
            combined_->setFixedSize(250, 150);
            combined_->SetOnClickCallback([=]() {
                separated_->SetSelected(false);
                settings_->SetMultiDisplayMode(MultiDisplayMode::kCombined);
            });
            layout->addWidget(combined_);

            layout->addStretch();

            root_layout->addLayout(layout);
        }

        root_layout->addStretch();
        setLayout(root_layout);
    }

    SettingsContent::~SettingsContent() {

    }

    void SettingsContent::OnContentShow() {
        AppContent::OnContentShow();
    }

    void SettingsContent::OnContentHide() {
        AppContent::OnContentHide();
    }

}