//
// Created by RGAA on 2023-08-22.
//

#ifndef SAILFISH_CLIENT_PC_WORKSPACECOVER_H
#define SAILFISH_CLIENT_PC_WORKSPACECOVER_H

#include <QWidget>
#include <QSize>

#include <memory>

#include "sdk/StreamItem.h"

namespace rgaa {

    class Context;
    class FloatButton;
    class FloatMenu;
    class DebugWidget;
    class SailfishSDK;

    class WorkspaceCover : public QWidget {
    public:

        explicit WorkspaceCover(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, const StreamItem& item, QWidget* parent = nullptr);
        ~WorkspaceCover() override;

        void paintEvent(QPaintEvent *event) override;

        void OnWindowResize();
        void OnWindowMove();

    private:

        void Resize(const QSize& size);
        void AdjustPosition();

    private:

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;
        std::shared_ptr<Context> context_ = nullptr;

        FloatButton* float_button_ = nullptr;
        FloatMenu* float_menu_ = nullptr;

        int mouse_pressed_task_id_ = -1;
        int clipboard_task_id_ = -1;
        int audio_task_id_ = -1;

        StreamItem item_;

        bool debug_showing_ = false;

        DebugWidget* debug_widget_ = nullptr;

    };

}
#endif //SAILFISH_CLIENT_PC_WORKSPACECOVER_H
