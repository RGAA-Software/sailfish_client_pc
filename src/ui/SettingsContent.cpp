//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

#include "widgets/SwitchButton.h"
#include "WidgetHelper.h"
#include "MultiDisplayModeWidget.h"

namespace rgaa {

    SettingsContent::SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        root_layout->addSpacing(20);
        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(50);

            auto label = new QLabel(this);
            label->setFixedSize(330, 40);
            label->setText(tr("FIX THE MENU AT TOP"));
            label->setStyleSheet("font-size:11pt;");
            layout->addWidget(label);

            auto switch_btn = new SwitchButton(this);
            switch_btn->setFixedSize(55, 25);
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
            label->setText(tr("MULTIPLE MONITORS DISPLAY MODE"));
            label->setStyleSheet("font-size:11pt;");
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
            auto separated = new MultiDisplayModeWidget(MultiDisplayMode::kSeparated, this);
            separated->setFixedSize(250, 150);
            layout->addWidget(separated);

            layout->addSpacing(20);

            auto combined = new MultiDisplayModeWidget(MultiDisplayMode::kCombined, this);
            combined->setFixedSize(250, 150);
            layout->addWidget(combined);

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