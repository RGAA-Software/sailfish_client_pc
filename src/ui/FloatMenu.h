//
// Created by RGAA on 2023-08-22.
//

#ifndef SAILFISH_CLIENT_PC_FLOATMENU_H
#define SAILFISH_CLIENT_PC_FLOATMENU_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <functional>

#include "rgaa_common/RLog.h"
#include "sdk/StreamItem.h"

namespace rgaa {

    class Context;

    class FloatMenuItem : public QWidget {
    public:

        FloatMenuItem(const QString& name, const QString& normal_icon_path, const QString& expand_icon_path = "", QWidget* parent = nullptr);
        ~FloatMenuItem();

        void SetOnClickCallback(std::function<void()>&& cbk);
        void UpdateTransparency(float v);

        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

        void SetExpand(bool st);
        bool GetExpand();

    private:

        std::function<void()> click_cbk_;

        bool enter_ = false;
        bool pressed_ = false;
        float transparency_ = 1.0f;

        QPixmap normal_icon_;
        QPixmap expand_icon_;

        QLabel* icon_ = nullptr;
        bool expand_ = false;

    };

    class FloatMenu : public QWidget {
    public:

        explicit FloatMenu(const std::shared_ptr<Context>& ctx, const StreamItem& item, QWidget* parent = nullptr);
        ~FloatMenu() override;

        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

        void Show();
        void ShowWithAnim();
        void Hide();
        void HideWithAnim(std::function<void()>&& finished_task = nullptr);

        void SetOnDebugStatusCallback(std::function<void(bool)>&& cbk);

    private:

        float transparency_ = 0.0f;

        FloatMenuItem* item_clipboard_ = nullptr;
        FloatMenuItem* item_audio_ = nullptr;
        std::vector<FloatMenuItem*> menu_items_;
        std::shared_ptr<Context> context_ = nullptr;
        StreamItem item_;

        std::function<void(bool)> debug_status_cbk_;

    };

}

#endif //SAILFISH_CLIENT_PC_FLOATMENU_H
