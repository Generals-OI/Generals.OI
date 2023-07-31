#ifndef WINDOW_FRAME_H
#define WINDOW_FRAME_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QStatusBar>

#include "titleBar.h"

class WindowFrame : public QMainWindow {
Q_OBJECT

public:
    explicit WindowFrame(QWidget *, QWidget * = nullptr);

    ~WindowFrame() override = default;

    void setTitle(const QString &);

    QWidget *mainWidget{};
    QVBoxLayout *verticalLayout{};
    TitleBar *titleBar{};
    QWidget *contentWidget{};
    QStatusBar *statusBar{};
};

#endif // WINDOW_FRAME_H
