//
// Created by RGAA on 2023-08-19.
//

#include "ClipboardManager.h"

#include <QObject>
#include <QTimer>

#include "SailfishSDK.h"
#include "rgaa_common/RLog.h"
#include "MessageMaker.h"

namespace rgaa {

    ClipboardManager::ClipboardManager(const std::shared_ptr<SailfishSDK> &sdk) {
        this->sdk_ = sdk;
    }

    ClipboardManager::~ClipboardManager() {
        StopMonitoringClipboard();
    }

    void ClipboardManager::Init() {
        clipboard_ = QApplication::clipboard();
        StartMonitoringClipboard();

//        std::thread* t = new std::thread([=](){
//            for (int i = 0; i < 10000; i++) {
//                auto msg = QString::number(i) + " //// test";
//                SetText(msg);
//                std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            }
//        });
//        t->detach();
    }

    void ClipboardManager::OnClipboardDataChanged() {
        QClipboard *clipboard = QApplication::clipboard();
        auto text = clipboard->text();
        if (text.isEmpty()) {
            return;
        }

        LOGI("clip board : {} {}", text.toStdString(), (void*)this);
        auto msg = MessageMaker::MakeClipboard(text.toStdString());
        sdk_->PostNetMessage(msg);
    }

    void ClipboardManager::SetText(const QString &msg) {
        QTimer::singleShot(0, this, [=, this](){
            StopMonitoringClipboard();
            clipboard_->setText(msg);
            StartMonitoringClipboard();
        });
    }

    void ClipboardManager::StartMonitoringClipboard() {
        QTimer::singleShot(0, this, [=, this]() {
            connect(clipboard_, &QClipboard::dataChanged, this,
                             &ClipboardManager::OnClipboardDataChanged);
        });
    }

    void ClipboardManager::StopMonitoringClipboard() {
        disconnect(clipboard_, &QClipboard::dataChanged, this,
                            &ClipboardManager::OnClipboardDataChanged);
    }
}