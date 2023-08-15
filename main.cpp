#include <iostream>
#include <thread>

#include "ui/Workspace.h"
#include "Context.h"
#include "ui/Application.h"

#include <QSurfaceFormat>

using namespace rgaa;

int main(int argc, char** argv) {

    QApplication app(argc, argv);

    auto ctx = std::make_shared<Context>();

    Application application(ctx);
    application.show();

    Workspace ws(ctx);
    ws.Run();


    return QApplication::exec();
}
