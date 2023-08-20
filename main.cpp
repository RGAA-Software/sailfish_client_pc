#include <iostream>
#include <thread>

#include "ui/Workspace.h"
#include "Context.h"
#include "ui/Application.h"
#include "rgaa_common/RDump.h"

#include <QSurfaceFormat>

using namespace rgaa;

int main(int argc, char** argv) {

    CaptureDump();

    QApplication app(argc, argv);

    auto ctx = std::make_shared<Context>();

    Application application(ctx);
    application.show();


    return QApplication::exec();
}
