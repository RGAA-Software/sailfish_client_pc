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
        clipboard_->clear();
        StopMonitoringClipboard();
    }

    void ClipboardManager::Init() {
        clipboard_ = QApplication::clipboard();
        connect(clipboard_, &QClipboard::dataChanged, this, &ClipboardManager::OnClipboardDataChanged);
    }

    void ClipboardManager::OnClipboardDataChanged() {
        QClipboard *clipboard = QApplication::clipboard();
        auto text = clipboard->text();
        if (!enable_) {
            LOGI("monitor, clipboard disabled.");
            return;
        }
        if (text.isEmpty() || text == manual_set_msg_) {
            LOGI("Manu set, ignore.");
            return;
        }

        if (text.startsWith("file:///")) {
            LOGI("Ignore file.");
            return;
        }

        LOGI("clip board : {} {}", text.toStdString(), (void*)this);
        auto msg = MessageMaker::MakeClipboard(text.toStdString());
        sdk_->PostNetMessage(msg);
    }

    void ClipboardManager::SetText(const QString &msg) {
        if (!enable_) {
            LOGI("setText, clipboard disabled.");
            return;
        }
        manual_set_msg_ = msg;
        QMetaObject::invokeMethod(this, [=]() {
            clipboard_->setText(msg);
        });
    }

    void ClipboardManager::StopMonitoringClipboard() {
        disconnect(clipboard_, &QClipboard::dataChanged, this, &ClipboardManager::OnClipboardDataChanged);
        disconnect(clipboard_, &QClipboard::dataChanged, nullptr, nullptr);
    }

    void ClipboardManager::Exit() {
        StopMonitoringClipboard();
    }

    void ClipboardManager::Enable() {
        enable_ = true;
    }

    void ClipboardManager::Disable() {
        enable_ = false;
    }
}