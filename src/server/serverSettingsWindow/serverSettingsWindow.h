#ifndef SERVER_SETTINGS_WINDOW_H
#define SERVER_SETTINGS_WINDOW_H

#include <QWidget>

#include "server.h"

namespace Ui {
    class ServerSettingsWindow;
}

class ServerSettingsWindow : public QWidget {
Q_OBJECT

public:
    explicit ServerSettingsWindow(QWidget * = nullptr);

    ~ServerSettingsWindow() override = default;

    void setTarget(QWidget *);

protected:
    void onCreateButtonClicked();

private:
    Ui::ServerSettingsWindow *ui;
    QWidget *wTarget{};
};

#endif // SERVER_SETTINGS_WINDOW_H
