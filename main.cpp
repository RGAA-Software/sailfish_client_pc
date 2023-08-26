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

#if 0
    int fontId = QFontDatabase::addApplicationFont(":/resources/font/SourceHanSansCN-Regular.otf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QFont customFont(fontFamilies.at(0), 16);
            customFont.setStyleStrategy(QFont::PreferAntialias);
            app.setFont(customFont);
        }
    }
#endif

    QFont font;
    font.setPointSize(10);
    qApp->setFont(font);

    auto ctx = std::make_shared<Context>();

    Application application(ctx);
    application.show();


    return QApplication::exec();
}
