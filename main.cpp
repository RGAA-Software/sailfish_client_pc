#include <iostream>
#include <thread>

#include "ui/Workspace.h"
#include "Context.h"
#include "ui/Application.h"
#include "rgaa_common/RDump.h"
#include "rgaa_common/RLog.h"

#include <QSurfaceFormat>
#include <QFontDatabase>

using namespace rgaa;

int main(int argc, char** argv) {

#ifdef WIN32
    CaptureDump();
#endif

#ifdef __APPLE__
    QSurfaceFormat fmt;
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setVersion(3, 3);
    fmt.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(fmt);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif

    //qputenv("QT_SCALE_FACTOR", QByteArray::number(2));
    QApplication app(argc, argv);

    auto id = QFontDatabase::addApplicationFont(":/resources/font/quixotic-1.otf");
    qDebug() << "font family : " << QFontDatabase::applicationFontFamilies(id) ;

    QFont font;
    font.setPointSize(10);
    qApp->setFont(font);

    auto ctx = std::make_shared<Context>();

    Application application(ctx);
    application.show();


    return QApplication::exec();
}
