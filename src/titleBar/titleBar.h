#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QFile>
#include <QMouseEvent>

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(QWidget *, int);
    ~TitleBar() = default;
    void setParent(QWidget *);

    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout;
    QFrame *fBorder;
    QLabel *lbIcon, *lbTitle;
    QSpacerItem *hSpacer;
    QPushButton *btnMinimize, *btnClose;

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    bool mPressed;
    QPoint pStart;
    QWidget *wParent;
};

#endif // TITLE_BAR_H
