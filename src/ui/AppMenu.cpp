//
// Created by RGAA on 2023/8/14.
//

#include "AppMenu.h"

#include <utility>

#include "Context.h"

namespace rgaa {

    AppMenu::AppMenu(const std::shared_ptr<Context> ctx, QWidget* parent) : QWidget(parent) {
        context_ = ctx;
        CreateLayout();
    }

    void AppMenu::CreateLayout() {
        setFixedHeight(60);
        auto btn_size = QSize(100, 30);
        auto root_layout = new QHBoxLayout();
        root_layout->setContentsMargins(0,0,0,0);
        root_layout->setSpacing(0);

        auto btn_add = new QPushButton(tr("Add"));
        btn_add->setFixedSize(btn_size);
        root_layout->addSpacing(20);
        root_layout->addWidget(btn_add);
        connect(btn_add, &QPushButton::clicked, this, [=, this]() {
            if (add_cbk_) {
                add_cbk_();
            }
        });

        auto btn_settings = new QPushButton(tr("Settings"));
        btn_settings->setFixedSize(btn_size);
        root_layout->addSpacing(20);
        root_layout->addWidget(btn_settings);

        root_layout->addStretch();

        setLayout(root_layout);
    }

    void AppMenu::SetOnAddCallback(OnAppMenuClickCallback cbk) {
        add_cbk_ = std::move(cbk);
    }

}