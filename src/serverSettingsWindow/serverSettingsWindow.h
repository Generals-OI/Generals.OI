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

protected:
    void onCreateButtonClicked();

private:
    Ui::ServerSettingsWindow *ui;
};

#endif // SERVER_SETTINGS_WINDOW_H
