//
// Created by RGAA on 2023-08-13.
//

#ifndef SAILFISH_CLIENT_PC_APPLICATION_H
#define SAILFISH_CLIENT_PC_APPLICATION_H

#include <QtWidgets/QMainWindow>

namespace rgaa {

    class Application : public QMainWindow {
    public:

        Application(QWidget* parent = nullptr);
        ~Application();


    };

}

#endif //SAILFISH_CLIENT_PC_APPLICATION_H
