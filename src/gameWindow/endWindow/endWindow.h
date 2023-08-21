#ifndef END_WINDOW_H
#define END_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QFile>

class EndWindow : public QWidget
{
Q_OBJECT
public:
    explicit EndWindow(QWidget *);

    QLabel *lbGeneral, *lbInfo;
    QPushButton *btnWatch, *btnExit;
};

#endif // END_WINDOW_H
