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
#include <QSoundEffect>
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
#include <cmath>

#include "point.h"
#include "gameButton.h"
#include "clientMap.h"
#include "gameInformation.h"
#include "endWindow.h"
#include "processJson.h"
#include "surrenderWindow.h"

#include <QDebug>

struct Focus;
struct MoveInfo;

class Highlighter;

class GameWindow;

const QString strColor[] = {
        "#DCDCDC",
        "#0288D1", "#E57373", "#9575CD", "#FF9800",
        "#FFCA28", "#8D6E63", "#66BB6A", "#4DB6AC",
        "#3949AB", "#D32F2F", "#5E35B1", "#FF5722",
        "#AFB42B", "#5D4037", "#2E7D32", "#00695C",
        "#FFFFFF"
};

/*
const QString strColor[] = {
        "#dcdcdc",
        "#ff0000", "#4363d8", "#008000", "#008080",
        "#800080", "#0000ff", "#f58231", "#483d8b",
        "#b09f30", "#f032e6", "#9a6324", "#7ab78c",
        "#800000"
};
*/

/*
const QString strColor[] = {
    "#dcdcdc",
    "#f07178", "#82aaff", "#c3e88d", "#008080",
    "#ffcb6b", "#c792ea", "#c792ea", "#f78c6c"
};
*/

class GameWindow : public QWidget, public ProcessJson {
Q_OBJECT

public:
    explicit GameWindow(QWebSocket *, QString, QWidget * = nullptr);

    ~GameWindow() override;

private:
    void init();

    void calcMapFontSize();

    void processMessage(const QByteArray &);

    void sendChatMessage();

    void updateFocus(bool, int, int = 0, int = 0);

    QRect mapPosition(int, int);

    void updateWindow(bool = false);

    void focusGeneral();

    void clearMove();

    void cancelMove(bool = false);

    void setGameFieldGeometry(QRect) const;

    bool isPositionVisible(int x, int y);

    void onSurrender();

    void onSpectate();

protected:
    void keyPressEvent(QKeyEvent *) override;

public slots:

    void onGameButtonFocused(const int &, const int &);

public:
    static const int fontSizeCount = 6;
    const Point dtDirection[4] = {Point(-1, 0), Point(1, 0), Point(0, -1), Point(0, 1)};
    const double mapFontSizePct[fontSizeCount] = {0.36, 0.28, 0.22, 0.18, 0.16, 0.13};

    int cntPlayer{};
    std::vector<PlayerInfo> playersInfo = std::vector<PlayerInfo>(maxPlayerNum + 5);

    int rnkUnitWidth{};
    int rnkWidth = 10, itvWidth = 2;

    QString nickName;
    QWebSocket *webSocket;
    QMediaPlayer *gongPlayer;
//    QSoundEffect *gongSoundEffect;

    bool gameWindowShown{}, gameEnded{}, surrendered{}, spectated{};

    qreal dpi;
    QRect screenGeometry;
    QLabel *lbMapBgd{};
    int screenWidth{}, screenHeight{};
    int width{}, height{};
    int unitSize{}, minUnitSize{};
    int chatFontSize{};

    QFont mapFont[fontSizeCount]{}, boardFont{}, chatFont{};
    std::vector<std::vector<int>> fontType;

    int mapLeft{}, mapTop{};
    int rnkLeft{}, rnkTop{};
    int idTeam{}, idPlayer{};
    int gameMode{};
    bool flagHalf{};

    Focus *focus{};
    QLabel *lbFocus{}, *lbShadow[4]{};
    std::vector<std::vector<GameButton *>> btnFocus;

    ClientMap cltMap{}, _cltMap{};

    QWidget *wgtMap{}, *wgtButton{}, *wgtBoard{};
    QGridLayout *mapLayout{}, *buttonLayout{}, *boardLayout{};

    std::vector<std::vector<QLabel *>> lbArrow[4];
    std::vector<std::vector<int>> cntArrow[4];

    std::vector<std::vector<QLabel *>> lbObstacle, lbColor, lbMain;
    std::vector<std::vector<bool>> visMain;

    std::deque<MoveInfo> dqMsg;

    struct BoardLabel;
    QVector<BoardLabel> lbBoard;
    QLabel *lbRound{};

    QTextEdit *teChats{};
    QLineEdit *leChat{};

    SurrenderWindow *surrenderWindow{};
    EndWindow *endWindow{};

    bool moved{};
    bool gotPlayerInfoMsg{}, gotInitMsg{}, gotPlayersInfoMsg{};
};

struct Focus : public Point {
    int width{}, height{};

    Focus();

    bool valid(int, int) const;

    void init(int, int);

    bool move(int, int);

    bool set(int, int);
};

struct GameWindow::BoardLabel {
    QLabel *lbName{}, *lbArmy{}, *lbLand{};

    void init(QWidget *parent, QFont &font, QGridLayout *layout, int row);

    void updateContent(const QString &strName = QString(), const QString &strArmy = QString(),
                       const QString &strLand = QString()) const;
};

class Highlighter : public QSyntaxHighlighter {
Q_OBJECT

public:
    explicit Highlighter(QTextDocument *, int &, std::vector<PlayerInfo> &, const QFont &);

    ~Highlighter() override = default;

protected:
    void highlightBlock(const QString &) override;

    void addRule(const QString &, QColor, const QFont &);

    static QString transExpr(const QString &);

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
