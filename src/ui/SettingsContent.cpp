//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

#include "widgets/SwitchButton.h"
#include "WidgetHelper.h"

namespace rgaa {

    SettingsContent::SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        root_layout->addSpacing(50);
        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            auto switch_btn = new SwitchButton(this);
            switch_btn->setFixedSize(70, 30);
            layout->addSpacing(30);
            layout->addWidget(switch_btn);
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