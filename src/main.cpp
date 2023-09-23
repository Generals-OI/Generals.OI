#include <QCommandLineParser>

#include "startWindow.h"
#include "serverSettingsWindow.h"
#include "replayWindow.h"
#include "windowFrame.h"

const QString strAppVersion = "1.0.0";

QString strFontRegular, strFontMedium, strFontBold;

int main(int argc, char *argv[]) {
#if (QT_VERSION_MAJOR < 6)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    QApplication::setApplicationName("Generals.OI");
    QApplication::setApplicationVersion(strAppVersion);
    QApplication::setWindowIcon(QIcon(":/img/Icon-Blue.ico"));

    QCommandLineParser clp;
    clp.addHelpOption();
    clp.addVersionOption();
    clp.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    QCommandLineOption cloServerMode("s", "Server mode");
    clp.addOption(cloServerMode);
    QCommandLineOption cloReplayMode("r", "Replay mode");
    clp.addOption(cloReplayMode);

    clp.process(app);

    auto idFontRegular = QFontDatabase::addApplicationFont(":/font/Quicksand-Regular.ttf");
    strFontRegular = QFontDatabase::applicationFontFamilies(idFontRegular).at(0);
    auto idFontMedium = QFontDatabase::addApplicationFont(":/font/Quicksand-Medium.ttf");
    strFontMedium = QFontDatabase::applicationFontFamilies(idFontMedium).at(0);
    auto idFontBold = QFontDatabase::addApplicationFont(":/font/Quicksand-Bold.ttf");
    strFontBold = QFontDatabase::applicationFontFamilies(idFontBold).at(0);

    qDebug() << "[main.cpp] Loaded font:" << strFontRegular << "id:" << idFontRegular;
    qDebug() << "[main.cpp] Loaded font:" << strFontMedium << "id:" << idFontMedium;
    qDebug() << "[main.cpp] Loaded font:" << strFontBold << "id:" << idFontBold;

    QFile cssFile(":/qss/WindowWidgets.qss");
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(cssFile.readAll());
        cssFile.close();
    }

    if (clp.isSet("s")) {
        qDebug() << "[main.cpp] Server mode enabled";
        QApplication::setQuitOnLastWindowClosed(false);

        auto serverSettingsWindow = new ServerSettingsWindow;
        auto serverSettingsWindowFrame = new WindowFrame(serverSettingsWindow);
        serverSettingsWindowFrame->setTitle("Generals.OI - Server Settings");
        serverSettingsWindow->setTarget(serverSettingsWindowFrame);
        serverSettingsWindowFrame->show();
    } else if (clp.isSet("r")) {
        auto replayWindow = new ReplayWindow;
        replayWindow->show();
    } else {
        qDebug() << "[main.cpp] Defaulting to client mode";

        auto startWindow = new StartWindow;
        auto startWindowFrame = new WindowFrame(startWindow);
        startWindowFrame->setTitle("Generals.OI");
        startWindow->setTarget(startWindowFrame);
        startWindowFrame->show();
    }

    return QApplication::exec();
}
