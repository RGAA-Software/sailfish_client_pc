//
// Created by RGAA on 2023/8/24.
//

#ifndef SAILFISH_CLIENT_PC_DEBUGWIDGET_H
#define SAILFISH_CLIENT_PC_DEBUGWIDGET_H

#include <QWidget>
#include <QTimer>

#include <functional>
#include <memory>

namespace rgaa {

    class Context;
    class Statistics;
    class LineChart;

    class DebugWidget : public QWidget {
    public:

        explicit DebugWidget(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~DebugWidget() override;

        void paintEvent(QPaintEvent *event) override;

    private:

        void UpdateStatistics();

    private:

        std::shared_ptr<Context> context_ = nullptr;
        Statistics* statistics_ = nullptr;
        LineChart* decode_time_chart_ = nullptr;

        QTimer* timer_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_DEBUGWIDGET_H
