#ifndef SERVER_SETTINGS_WINDOW_H
#define SERVER_SETTINGS_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerSettingsWindow; }
QT_END_NAMESPACE

class ServerSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerSettingsWindow(QWidget * = nullptr);
    ~ServerSettingsWindow();

private:
    Ui::ServerSettingsWindow *ui;
};

#endif // SERVER_SETTINGS_WINDOW_H
