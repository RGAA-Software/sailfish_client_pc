//
// Created by RGAA on 2023-08-22.
//

#include "WorkspaceCover.h"

#include <QPalette>
#include <QVBoxLayout>
#include <QPushButton>

#include "WidgetHelper.h"
#include "FloatButton.h"
#include "FloatMenu.h"
#include "Context.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RMessageQueue.h"
#include "AppMessage.h"

namespace rgaa {

    WorkspaceCover::WorkspaceCover(const std::shared_ptr<Context>& ctx, QWidget* parent) : QWidget(parent) {
        this->context_ = ctx;
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
        setAttribute(Qt::WA_TranslucentBackground);

        auto root_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(root_layout);

        auto float_btn_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(float_btn_layout);
        float_btn_layout->addStretch();
        auto float_btn = new FloatButton(this);
        float_button_ = float_btn;
        float_button_->ShowWithAnim();
        float_button_->SetOnClickCallback([=, this]() {
            float_menu_->ShowWithAnim();
            float_button_->Hide();
        });

        float_btn->setFixedSize(100, 26);
        float_btn_layout->addWidget(float_btn);
        float_btn_layout->addStretch();
        root_layout->addLayout(float_btn_layout);

        auto float_menu_layout = new QHBoxLayout();
        float_menu_layout->addStretch();

        auto float_menu = new FloatMenu(this);
        float_menu_ = float_menu;
        float_menu_->hide();
        float_menu->setFixedSize(400, 60);
        float_menu_layout->addWidget(float_menu);

        float_menu_layout->addStretch();
        root_layout->addLayout(float_menu_layout);

        root_layout->addStretch();
        setLayout(root_layout);

        mouse_pressed_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeMousePressed, [=, this](auto& msg) {
            context_->PostUITask([=]() {
                if (!float_menu_->isHidden()) {
                    float_menu_->HideWithAnim([=](){
                        float_button_->ShowWithAnim();
                    });
                }
            });
        }));
    }

    WorkspaceCover::~WorkspaceCover() {
        LOGI("~~~WorkspaceCover");
        context_->RemoveMessageTask(mouse_pressed_task_id_);
    }



}
