//
// Created by RGAA on 2023-08-13.
//

#include "Application.h"

#include <QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QElapsedTimer>
#include <QFile>

#include "AppMenu.h"
#include "AppStreamList.h"
#include "Workspace.h"
#include "rgaa_common/RLog.h"

namespace rgaa {

    Application::Application(const std::shared_ptr<Context>& ctx, QWidget* parent) : QMainWindow(parent) {
        context_ = ctx;
        resize(1280, 768);
        setWindowTitle(tr("Sailfish Client"));
        CreateLayout();

        LoadStyle("");
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
        app_menu->SetOnAddCallback([=, this]() {
            StartStreaming();
        });

        // 2. stream list
        auto stream_list = new AppStreamList(context_, this);
        root_layout->addWidget(stream_list);

        root_widget->setLayout(root_layout);
        setCentralWidget(root_widget);
    }

    void Application::LoadStyle(const std::string &name) {
        QElapsedTimer time;
        time.start();

        auto qssFile = ":/qss/lightblue.css";

        QString qss;
        QFile file(qssFile);
        if (file.open(QFile::ReadOnly)) {
            qDebug() << "open success...";
            QStringList list;
            QTextStream in(&file);
            //in.setCodec("utf-8");
            while (!in.atEnd()) {
                QString line;
                in >> line;
                list << line;
            }

            file.close();
            qss = list.join("\n");
            QString paletteColor = qss.mid(20, 7);
            qApp->setPalette(QPalette(paletteColor));
            qApp->setStyleSheet(qss);
        }

        qDebug() << "用时:" << time.elapsed();
    }

    void Application::StartStreaming() {
        workspace_ = std::make_shared<Workspace>(context_);
        workspace_->SetOnCloseCallback([=, this]() {
            workspace_.reset();
            workspace_ = nullptr;
            LOGI("Workspace closed...");
        });
        workspace_->Run();
    }

}