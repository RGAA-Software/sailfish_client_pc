//
// Created by RGAA on 2023/8/14.
//

#ifndef SAILFISH_CLIENT_PC_APPSTREAMLIST_H
#define SAILFISH_CLIENT_PC_APPSTREAMLIST_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QPaintEvent>

#include "sdk/StreamItem.h"

namespace rgaa {

    class Context;
    class StreamDBManager;

    using OnItemDoubleClickedCallback = std::function<void(const StreamItem&)>;

    class AppStreamList : public QWidget {
    public:

        explicit AppStreamList(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~AppStreamList();

        void paintEvent(QPaintEvent *event) override;

        void SetOnItemDoubleClickedCallback(OnItemDoubleClickedCallback&& cbk);

    private:
        QListWidgetItem* AddItem(const StreamItem& item);
        void RegisterActions(int index);
        void ProcessAction(int index, const StreamItem& item);

        void CreateLayout();
        void Init();

        void LoadStreamItems();

    private:

        void DeleteStream(const StreamItem& item);

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<StreamDBManager> db_mgr_ = nullptr;
        std::vector<StreamItem> streams_;

        QListWidget* stream_list_ = nullptr;

        int stream_added_task_id_ = -1;

        OnItemDoubleClickedCallback dbk_callback_;

    };

}

#endif //SAILFISH_CLIENT_PC_APPSTREAMLIST_H
