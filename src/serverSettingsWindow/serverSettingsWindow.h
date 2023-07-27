#ifndef SERVER_SETTINGS_WINDOW_H
#define SERVER_SETTINGS_WINDOW_H

#include <QMainWindow>

#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ServerSettingsWindow;
}
QT_END_NAMESPACE

class ServerSettingsWindow : public QMainWindow {
Q_OBJECT

public:
    ServerSettingsWindow(QWidget * = nullptr);

    ~ServerSettingsWindow();

protected:
    void onCreateButtonClicked();

private:
    Ui::ServerSettingsWindow *ui;
};

#endif // SERVER_SETTINGS_WINDOW_H
