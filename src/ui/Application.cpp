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
#include "rgaa_common/RThread.h"
#include "CreateStreamDialog.h"
#include "Context.h"
#include "rgaa_common/RMessageQueue.h"
#include "AppMessage.h"
#include "WidgetHelper.h"

namespace rgaa {

    Application::Application(const std::shared_ptr<Context>& ctx, QWidget* parent) : QMainWindow(parent) {
        context_ = ctx;
        resize(1280, 768);
        setWindowTitle(tr("Sailfish Client"));

        CreateLayout();
        Init();

        LoadStyle("");
    }

    Application::~Application() {
        if (clear_ws_task_id_ != -1) {
            context_->RemoveMessageTask(clear_ws_task_id_);
        }
        if (clear_ws_task_id_ != -1) {
            context_->RemoveMessageTask(close_ws_task_id_);
        }
    }

    void Application::CreateLayout() {
        auto root_widget = new QWidget(this);
        auto root_layout = new QVBoxLayout();
        root_layout->setSpacing(0);
        root_layout->setContentsMargins(0,0,0,0);
        WidgetHelper::ClearMargin(root_layout);

        // 1. app menu
        auto app_menu = new AppMenu(context_, this);
        root_layout->addWidget(app_menu);
        app_menu->SetOnAddCallback([=, this]() {
            CreateStreamDialog dialog(context_);
            dialog.exec();
        });

        // 2. stream list
        auto stream_list = new AppStreamList(context_, this);
        stream_list_ = stream_list;
        root_layout->addWidget(stream_list);

        root_widget->setLayout(root_layout);
        setCentralWidget(root_widget);
    }

    void Application::Init() {
        stream_list_->SetOnItemDoubleClickedCallback([=, this](const StreamItem& item) {
            StartStreaming(item);
        });

        clear_ws_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeClearWorkspace, [=, this](auto& msg) {
            context_->PostUITask([=]() {
                auto target_msg = std::dynamic_pointer_cast<ClearWorkspace>(msg);
                auto stream_id = target_msg->item_.stream_id;
                LOGI("CLEAR:The stream's id : {}, will be release.", stream_id);
                if (workspaces_.find(target_msg->item_.stream_id) != workspaces_.end()) {
                    LOGI("CLEAR: Find the ws, will release it : {}", stream_id);
                    auto ws = workspaces_[stream_id];
                    if (!ws->CloseWorkspace()) {
                        return;
                    }
                    workspaces_.erase(stream_id);
                    ws.reset();
                    LOGI("CLEAR: Workspace closed, stream id : {}", stream_id);
                }
            });
        }));

        close_ws_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeCloseWorkspace, [=, this](auto& msg) {
            context_->PostUITask([=]() {
                auto target_msg = std::dynamic_pointer_cast<CloseWorkspace>(msg);
                auto stream_id = target_msg->item_.stream_id;
                if (workspaces_.find(target_msg->item_.stream_id) != workspaces_.end()) {
                    LOGI("CLOSE: Find the ws, will release it : {}", stream_id);
                    auto ws = workspaces_[stream_id];
                    workspaces_.erase(stream_id);
                    ws.reset();
                    LOGI("CLOSE: Workspace closed, stream id : {}", stream_id);
                }
            });
        }));

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

    void Application::StartStreaming(const StreamItem& item) {
        std::shared_ptr<Workspace> workspace = std::make_shared<Workspace>(context_, item);
//        workspace->SetOnCloseCallback([=, this]() {
//            auto stream_id = workspace->GetStreamItem().stream_id;
//            if (workspaces_.find(stream_id) != workspaces_.end()) {
//                workspaces_[stream_id].reset();
//                workspaces_.erase(stream_id);
//                LOGI("Workspace closed, stream id : {}", stream_id);
//            }
//        });
        workspace->Run();

        workspaces_.insert(std::make_pair(item.stream_id, workspace));
    }

}