//
// Created by RGAA on 2023-08-13.
//

#include "Application.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

#include "AppMenu.h"
#include "AppStreamList.h"

namespace rgaa {

    Application::Application(const std::shared_ptr<Context>& ctx, QWidget* parent) : QMainWindow(parent) {
        context_ = ctx;
        resize(1280, 768);
        setWindowTitle(tr("Sailfish Client"));
        CreateLayout();
    }

    Application::~Application() {

    }

    void Application::CreateLayout() {
        auto root_widget = new QWidget(this);
        auto root_layout = new QVBoxLayout(this);
        root_layout->setSpacing(0);
        root_layout->setContentsMargins(0,0,0,0);

        // 1. app menu
        auto app_menu = new AppMenu(context_, this);
        root_layout->addWidget(app_menu);

        // 2. stream list
        auto stream_list = new AppStreamList(context_, this);
        root_layout->addWidget(stream_list);

        root_widget->setLayout(root_layout);
        setCentralWidget(root_widget);
    }


}