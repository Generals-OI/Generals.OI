#ifndef REPLAY_WINDOW_H
#define REPLAY_WINDOW_H

#include "gameWindow.h"
#include "recorder.h"

#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class ControlPanel; }
QT_END_NAMESPACE

class ReplayWindow : public QWidget, public ProcessJson {
Q_OBJECT

public:
    explicit ReplayWindow(QWidget *parent = nullptr);

    ~ReplayWindow() override;

protected:
    void updateSettings();

    void updateStatus();

    void sendMap();

private:
    QWebSocket *socket;
    QTimer *timer;
    GameWindow *gameWindow;

    QWidget *wCtrlPanel;
    Ui::ControlPanel *uiCtrlPanel;

    Recorder recorder;

    double speed = 1.0;
    bool paused{};
};

#endif // REPLAY_WINDOW_H
