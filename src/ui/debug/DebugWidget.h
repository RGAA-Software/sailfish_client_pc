//
// Created by RGAA on 2023/8/24.
//

#ifndef SAILFISH_CLIENT_PC_DEBUGWIDGET_H
#define SAILFISH_CLIENT_PC_DEBUGWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QGridLayout>
#include <QLabel>

#include <functional>
#include <memory>

namespace rgaa {

    class Context;
    class Statistics;
    class LineChart;
    class SailfishSDK;

    class DebugWidget : public QWidget {
    public:

        explicit DebugWidget(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, QWidget* parent = nullptr);
        ~DebugWidget() override;

        void paintEvent(QPaintEvent *event) override;

    private:

        void UpdateStatistics();

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;
        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<Statistics> statistics_ = nullptr;
        LineChart* decode_time_chart_ = nullptr;
        LineChart* video_frame_time_chart_ = nullptr;
        LineChart* encode_time_chart_ = nullptr;
        LineChart* network_time_chart_ = nullptr;

        QTimer* timer_ = nullptr;

        QGridLayout* right_grid_ = nullptr;
        QLabel* recv_fps_ = nullptr;
        QLabel* recv_video_bytes_ = nullptr;
        QLabel* video_encode_format_ = nullptr;
        QLabel* video_width_ = nullptr;
        QLabel* video_height_ = nullptr;
        QLabel* streaming_time_ = nullptr;

        QLabel* audio_bytes_ = nullptr;
        QLabel* audio_channel_ = nullptr;
        QLabel* audio_samples_ = nullptr;
        QLabel* audio_format_ = nullptr;
        QLabel* audio_bits_ = nullptr;
    };

}

#endif //SAILFISH_CLIENT_PC_DEBUGWIDGET_H
