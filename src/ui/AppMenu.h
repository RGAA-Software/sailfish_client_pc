//
// Created by RGAA on 2023/8/14.
//

#ifndef SAILFISH_CLIENT_PC_APPMENU_H
#define SAILFISH_CLIENT_PC_APPMENU_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace rgaa {

    class Context;

    using OnAppMenuClickCallback = std::function<void()>;

    class AppMenu : public QWidget {
    public:

        AppMenu(const std::shared_ptr<Context> ctx, QWidget* parent = nullptr);
        ~AppMenu() = default;

        void SetOnAddCallback(OnAppMenuClickCallback cbk);

    private:

        void CreateLayout();

    private:

        std::shared_ptr<Context> context_ = nullptr;

        OnAppMenuClickCallback add_cbk_;

    };

}

#endif //SAILFISH_CLIENT_PC_APPMENU_H
