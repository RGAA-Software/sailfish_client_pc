//
// Created by RGAA on 2023/8/14.
//

#include "AppStreamList.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

namespace rgaa {

    AppStreamList::AppStreamList(const std::shared_ptr<Context> ctx, QWidget* parent) : QWidget(parent) {
        context_ = ctx;

        CreateLayout();

    }

    void AppStreamList::paintEvent(QPaintEvent *event) {
        QWidget::paintEvent(event);

        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(0x07a7374)));
        painter.drawRect(0, 0, width(), height());

    }

    void AppStreamList::CreateLayout() {
        auto root_layout = new QHBoxLayout(this);


        setLayout(root_layout);
    }

}