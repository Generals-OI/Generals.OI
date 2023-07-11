#include "server.h"
#include "startWindow.h"

#include <QCommandLineParser>

QString strFontRegular, strFontMedium, strFontBold;

int main(int argc, char *argv[]) {
#if (QT_VERSION_MAJOR < 6)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    QApplication::setApplicationName("Generals.OI");
    QApplication::setApplicationVersion("0.1.1");

    QCommandLineParser clp;
    clp.addHelpOption();
    clp.addVersionOption();
    clp.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    QCommandLineOption cloNoServer("n", "No server mode");
    clp.addOption(cloNoServer);

    clp.process(app);

    Server *server;
    if (!clp.isSet("n")) {
        server = new Server;
    } else {
        qInfo() << "No server mode enabled";
    }

    auto idFontRegular = QFontDatabase::addApplicationFont(":/font/Quicksand-Regular.ttf");
    strFontRegular = QFontDatabase::applicationFontFamilies(idFontRegular).at(0);
    auto idFontMedium = QFontDatabase::addApplicationFont(":/font/Quicksand-Medium.ttf");
    strFontMedium = QFontDatabase::applicationFontFamilies(idFontMedium).at(0);
    auto idFontBold = QFontDatabase::addApplicationFont(":/font/Quicksand-Bold.ttf");
    strFontBold = QFontDatabase::applicationFontFamilies(idFontBold).at(0);

    qDebug() << idFontRegular << idFontMedium << idFontBold;
    qDebug() << strFontRegular << strFontMedium << strFontBold;

    StartWindow startWindow;
    startWindow.show();

    return QApplication::exec();
}
