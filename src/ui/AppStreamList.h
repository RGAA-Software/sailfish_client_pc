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

namespace rgaa {

    class Context;
    class StreamItem;
    class StreamDBManager;

    class AppStreamList : public QWidget {
    public:

        AppStreamList(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~AppStreamList() = default;

        void paintEvent(QPaintEvent *event) override;


    private:
        QListWidgetItem* AddItem(const std::shared_ptr<StreamItem>& item);
        void RegisterActions(int index);
        void ProcessAction(int index, const std::shared_ptr<StreamItem>& item);

        void CreateLayout();

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<StreamDBManager> db_mgr_ = nullptr;
        std::vector<std::shared_ptr<StreamItem>> streams_;

        QListWidget* stream_list_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_APPSTREAMLIST_H
