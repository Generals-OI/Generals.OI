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

#include "gameMapGrid.h"
#include "clientMap.h"
#include "gameInformation.h"
#include "endWindow.h"
#include "processJson.h"
#include "surrenderWindow.h"

#include <QDebug>

struct Focus;

class Highlighter;

class GameWindow;

extern const int maxPlayerNum;

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
    explicit GameWindow(QWebSocket *socket, QWidget *parent = nullptr);

    ~GameWindow() override;

    void setNickname(const QString &newNickname);

    void setIdentity(int identity);

private:
    void init();

    void calcMapFontSize();

    void processMessage(const QByteArray &msg);

    void sendChatMessage();

    void updateFocus(bool clicked, int id = 0, int x = 0, int y = 0);

    QRect mapPosition(int x, int y);

    void updateWindow(bool forced = false);

    void focusGeneral();

    void sendMove();

    void clearMove();

    void cancelMove(bool flagFront = false);

    void setGameFieldGeometry(QRect geometry) const;

    bool isPositionVisible(int x, int y);

    void onSurrender();

    void onSpectate();

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:

    void onGameButtonFocused(const int &x, const int &y);

public:
    static const int fontSizeCount = 6;
    const Point dtDirection[4] = {Point(-1, 0), Point(1, 0), Point(0, -1), Point(0, 1)};
    const double mapFontSizePct[fontSizeCount] = {0.36, 0.28, 0.22, 0.18, 0.16, 0.13};

    int cntPlayer{};
    std::vector<PlayerInfo> playersInfo = std::vector<PlayerInfo>(maxPlayerNum + 5);

    int rnkWidth = 10, itvWidth = 2;

    QString nickName;
    QWebSocket *webSocket;
    QMediaPlayer *gongPlayer;
//    QSoundEffect *gongSoundEffect;

    bool gameWindowShown{}, gameEnded{}, surrendered{}, spectated{};

    qreal dpi;
    QRect wndGeometry;
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
    bool isSpec{}, isRep{};

    Focus *focus{};

    ClientMap cltMap{}, _cltMap{};

    GameMapGrid *gameMapGrid{};
    QWidget *wgtBoard{};
    QGridLayout *boardLayout{};

    std::vector<std::vector<int>> cntArrow[4];

    std::vector<std::vector<bool>> visMain;

    std::deque<MoveInfo> dqMsg;

    int sumRow{};
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

    bool valid(int _x, int _y) const;

    void init(int _width, int _height);

    bool move(int dx, int dy);

    bool set(int _x, int _y);
};

struct GameWindow::BoardLabel {
    QLabel *lbName{}, *lbArmy{}, *lbLand{};

    static QLabel *create(QWidget *parent, QFont &font);

    void init(QWidget *parent, QFont &font, QGridLayout *layout, int row);

    void updateContent(const QString &strName = QString(), const QString &strArmy = QString(),
                       const QString &strLand = QString()) const;
};

class Highlighter : public QSyntaxHighlighter {
Q_OBJECT

public:
    explicit Highlighter(QTextDocument *parent, int &cntPlayer, std::vector<PlayerInfo> &playersInfo,
                         const QFont &font);

    ~Highlighter() override = default;

protected:
    void highlightBlock(const QString &text) override;

    void addRule(const QString &str, QColor color, const QFont &font);

    static QString transExpr(const QString &str);

private:
    struct HighlightingRule {
        QRegularExpression expr;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> rules;
};

#endif // GAME_WINDOW_H
