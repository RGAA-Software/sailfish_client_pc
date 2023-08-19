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
#include <QString>

#include <atomic>

namespace rgaa {

    class SailfishSDK;

    class ClipboardManager : public QObject {
        //Q_OBJECT
    public:

        explicit ClipboardManager(const std::shared_ptr<SailfishSDK>& ctx);
        ~ClipboardManager();

        void Init();
        void SetText(const QString& msg);

    private slots:
        void OnClipboardDataChanged();

    private:

        void StartMonitoringClipboard();
        void StopMonitoringClipboard();

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        QClipboard* clipboard_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H
