//
// Created by RGAA on 2023-08-19.
//

#ifndef SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H
#define SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H

#include <memory>

#include <QObject>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

namespace rgaa {

    class SailfishSDK;

    class ClipboardManager : public QObject {
    public:

        explicit ClipboardManager(const std::shared_ptr<SailfishSDK>& ctx);
        ~ClipboardManager();

        void Init();

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H
