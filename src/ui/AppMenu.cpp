//
// Created by RGAA on 2023/8/14.
//

#include "AppMenu.h"

#include "Context.h"

namespace rgaa {

    AppMenu::AppMenu(const std::shared_ptr<Context> ctx, QWidget* parent) : QWidget(parent) {
        context_ = ctx;
        CreateLayout();
    }

    void AppMenu::CreateLayout() {
        setFixedHeight(50);
        auto root_layout = new QHBoxLayout(this);
        root_layout->setContentsMargins(0,0,0,0);
        root_layout->setSpacing(0);

        auto btn_add = new QPushButton(tr("Add"));
        btn_add->resize(100, 40);
        root_layout->addWidget(btn_add);

        auto btn_settings = new QPushButton(tr("Settings"));
        btn_settings->resize(100, 40);
        root_layout->addWidget(btn_settings);

        root_layout->addStretch();

        setLayout(root_layout);
    }

}