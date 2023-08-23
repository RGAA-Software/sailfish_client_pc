//
// Created by RGAA on 2023-08-22.
//

#ifndef SAILFISH_CLIENT_PC_FLOATMENU_H
#define SAILFISH_CLIENT_PC_FLOATMENU_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <functional>

namespace rgaa {

    class FloatMenuItem : public QWidget {
    public:

        FloatMenuItem(QWidget* parent = nullptr);
        ~FloatMenuItem();

        void SetOnClickCallback(std::function<void()>&& cbk);
        void UpdateTransparency(float v) {
            transparency_ = v;
        }

        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

    private:

        std::function<void()> click_cbk_;

        bool enter_ = false;
        bool pressed_ = false;
        float transparency_ = 1.0f;

    };

    class FloatMenu : public QWidget {
    public:

        explicit FloatMenu(QWidget* parent = nullptr);
        ~FloatMenu();

        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

        void ShowWithAnim();
        void Hide();
        void HideWithAnim(std::function<void()>&& finished_task = nullptr);

    private:

        float transparency_ = 0.0f;

        std::vector<FloatMenuItem*> menu_items_;
    };

}

#endif //SAILFISH_CLIENT_PC_FLOATMENU_H
