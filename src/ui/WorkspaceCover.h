//
// Created by RGAA on 2023-08-22.
//

#ifndef SAILFISH_CLIENT_PC_WORKSPACECOVER_H
#define SAILFISH_CLIENT_PC_WORKSPACECOVER_H

#include <QWidget>

#include <memory>

namespace rgaa {

    class Context;
    class FloatButton;
    class FloatMenu;

    class WorkspaceCover : public QWidget {
    public:

        explicit WorkspaceCover(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~WorkspaceCover();

    private:

        std::shared_ptr<Context> context_ = nullptr;

        FloatButton* float_button_ = nullptr;
        FloatMenu* float_menu_ = nullptr;

        int mouse_pressed_task_id_ = -1;

    };

}
#endif //SAILFISH_CLIENT_PC_WORKSPACECOVER_H