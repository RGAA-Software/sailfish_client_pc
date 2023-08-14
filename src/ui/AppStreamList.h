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
#include <QPaintEvent>

namespace rgaa {

    class Context;

    class UIStreamItem {
    public:
        int width_;
        int height_;
        int top_;
        int left_;
        int bg_color_ = 0x980234;
    };
    using UIStreamItemPtr = std::shared_ptr<UIStreamItem>;

    class AppStreamList : public QWidget {
    public:

        AppStreamList(const std::shared_ptr<Context> ctx, QWidget* parent = nullptr);
        ~AppStreamList() = default;

        void paintEvent(QPaintEvent *event) override;

    private:

        void CreateLayout();

    private:

        std::shared_ptr<Context> context_ = nullptr;

        std::vector<UIStreamItemPtr> items_;

    };

}

#endif //SAILFISH_CLIENT_PC_APPSTREAMLIST_H
