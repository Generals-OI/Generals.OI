#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QMainWindow>
#include <QApplication>
#include <QScreen>
#include <QRect>
#include <QKeyEvent>
#include <QColor>
#include <QtWebSockets>
#include <QMediaPlayer>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QRegularExpression>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSizePolicy>

#if (QT_VERSION_MAJOR == 6)

#include <QAudioOutput>

#endif

#include <utility>
#include <queue>

#include "point.h"
#include "gameButton.h"
#include "globalMap.h"
#include "playerInfo.h"
#include "endWindow.h"

#include <QDebug>

#define MaxPlayerNum 8

struct Focus;
struct MoveInfo;

class Highlighter;

class GameWindow;

const QString strColor[] = {
        "#dcdcdc",
        "#ff0000", "#4363d8", "#008000", "#008080",
        "#800080", "#0000ff", "#f58231", "#483d8b",
        "#b09f30", "#f032e6", "#9a6324", "#7ab78c",
        "#800000"
};

/*
const QString strColor[] = {
    "#dcdcdc",
    "#f07178", "#82aaff", "#c3e88d", "#008080",
    "#ffcb6b", "#c792ea", "#c792ea", "#f78c6c"
};
*/

class GameWindow : public QWidget {
Q_OBJECT

public:
    explicit GameWindow(QWebSocket *, QString, QWidget * = nullptr);

    ~GameWindow() override;

private:
    void init();

    void calcMapFontSize();

    void transfer();

    void processMessage(const QString &);

    void sendChatMessage();

    void updateFocus(bool, int, int = 0, int = 0);

    QRect mapPosition(const int, const int);

    void updateWindow(bool = false);

    void clearMove();

    void cancelMove(bool = false);

protected:
    void keyPressEvent(QKeyEvent *) override;

signals:

    void windowReadied();

    void gameStarted();

    void gameEnded(bool);

public slots:

    void onGameButtonFocused(const int &, const int &);

public:
    const Point dtDirection[4] = {Point(-1, 0), Point(1, 0), Point(0, -1), Point(0, 1)};
    const double mapFontSizePct[4] = {0.36, 0.3, 0.25, 0.2};

    int cntPlayer{};
    std::vector<PlayerInfo> playersInfo = std::vector<PlayerInfo>(MaxPlayerNum + 5);

    int rnkUnitWidth{};
    int rnkWidth = 8, itvWidth = 2;

    QString nickName;
    QWebSocket *webSocket;
    QMediaPlayer *gongPlayer;

    qreal dpi;
    QRect screenGeometry;
    QLabel *lbMapBgd{};
    int screenWidth{}, screenHeight{};
    int width{}, height{};
    int unitSize{}, minUnitSize{};
    int chatFontSize{};

    QFont mapFont[4]{}, boardFont{}, chatFont{};
    std::vector<std::vector<int>> fontType;

    int mapLeft{}, mapTop{};
    int rnkLeft{}, rnkTop{};
    int idTeam{}, idPlayer{};
    bool flagHalf{};

    Focus *focus{};
    QLabel *lbFocus{}, *lbShadow[4]{};
    std::vector<std::vector<GameButton *>> btnFocus;
//    std::vector<std::vector<QPushButton *>> btnFocus;

    GlobalMap globMap{}, _globMap{};

    QWidget *wgtMap{}, *wgtRank{}, *wgtChat{};
    QGridLayout *mapLayout{}, *rankLayout{};
    QVBoxLayout *chatLayout{};

    std::vector<std::vector<QLabel *>> lbArrow[4];
    std::vector<std::vector<int>> cntArrow[4];

    std::vector<std::vector<QLabel *>> lbObstacle, lbColor, lbMain;
    std::vector<std::vector<bool>> visMain;

    std::deque<MoveInfo> dqMsg;

    std::vector<QLabel *> lbName, lbLand, lbArmy;
    QLabel *lbRound{};

    QTextEdit *teChats{};
    QLineEdit *leChat{};
    Highlighter *highlighter{};

    EndWindow *endWindow{};

    bool moved = false;
    bool gotPlayerInfo = false, gotInitMap = false, gotPlayerCnt = false;
    int gotPlayersInfo = 0;
};

struct Focus : public Point {
    int width{}, height{};

    Focus();

    bool valid(int, int);

    void init(int, int);

    bool move(int, int);

    bool set(int, int);
};

class Highlighter : public QSyntaxHighlighter {
Q_OBJECT

public:
    explicit Highlighter(QTextDocument *, int &, std::vector<PlayerInfo> &);

    ~Highlighter() override = default;

protected:
    void highlightBlock(const QString &) override;

    QString transExpr(const QString &);

private:
    struct HighlightingRule {
        QRegularExpression expr;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> rules;
};

struct MoveInfo {
    int startX, startY, direction;
    bool flag50p;

    MoveInfo();

    MoveInfo(Point, int, bool);

    MoveInfo(int, int, int, bool);
};

#endif // GAME_WINDOW_H
