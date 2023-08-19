//
// Created by RGAA on 2023-08-19.
//

#include "ClipboardManager.h"

#include "SailfishSDK.h"
#include "rgaa_common/RLog.h"
#include <QObject>
#include <QTimer>

namespace rgaa {

    ClipboardManager::ClipboardManager(const std::shared_ptr<SailfishSDK> &sdk) {
        this->sdk_ = sdk;
    }

    ClipboardManager::~ClipboardManager() {

    }

    void ClipboardManager::Init() {
        clipboard_ = QApplication::clipboard();
        StartMonitoringClipboard();


        SetText("This is text...");
    }

    void ClipboardManager::OnClipboardDataChanged() {
        QClipboard *clipboard = QApplication::clipboard();
        auto text = clipboard->text();
        if (text.isEmpty()) {
            return;
        }

        LOGI("clip board : {} {}", text.toStdString(), (void*)this);
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