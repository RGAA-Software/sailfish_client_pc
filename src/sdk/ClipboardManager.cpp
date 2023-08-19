//
// Created by RGAA on 2023-08-19.
//

#include "ClipboardManager.h"

#include "SailfishSDK.h"
#include "rgaa_common/RLog.h"
#include <QObject>

namespace rgaa {

    ClipboardManager::ClipboardManager(const std::shared_ptr<SailfishSDK>& sdk) {
        this->sdk_ = sdk;
    }

    ClipboardManager::~ClipboardManager() {

    }

    void ClipboardManager::Init() {

        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText("hello，我是QT复制到剪切板的文本.");
        QObject::connect(clipboard, &QClipboard::dataChanged, this, [=, this] () {
            auto text = clipboard->text();
            if (!text.isEmpty()) {
                LOGI("clip board : {}", text.toStdString());
                return;
            }
        });
    }

}
