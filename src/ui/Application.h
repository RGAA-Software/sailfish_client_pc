//
// Created by RGAA on 2023-08-13.
//

#ifndef SAILFISH_CLIENT_PC_APPLICATION_H
#define SAILFISH_CLIENT_PC_APPLICATION_H

#include <QtWidgets/QMainWindow>

namespace rgaa {

    class Context;
    class Workspace;

    class Application : public QMainWindow {
    public:

        Application(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~Application();

    private:

        void StartStreaming();

        void CreateLayout();

        void LoadStyle(const std::string& name);

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<Workspace> workspace_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_APPLICATION_H