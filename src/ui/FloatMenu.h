//
// Created by RGAA on 2023-08-22.
//

#ifndef SAILFISH_CLIENT_PC_FLOATMENU_H
#define SAILFISH_CLIENT_PC_FLOATMENU_H

#include <QWidget>

namespace rgaa {

    class FloatMenu : public QWidget {
    public:

        explicit FloatMenu(QWidget* parent = nullptr);
        ~FloatMenu();

        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void paintEvent(QPaintEvent *event) override;


    };

}

#endif //SAILFISH_CLIENT_PC_FLOATMENU_H
