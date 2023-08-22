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
    explicit TitleBar(QWidget *, int = 0);

    ~TitleBar() override = default;

    void setTarget(QWidget *);

    QHBoxLayout *hLayout;
    QLabel *lbIcon, *lbTitle;
    QSpacerItem *hSpacer;
    QPushButton *btnMinimize, *btnClose;

protected:
    void mousePressEvent(QMouseEvent *) override;

    void mouseReleaseEvent(QMouseEvent *) override;

    void mouseMoveEvent(QMouseEvent *) override;

    bool mPressed{};
    QPoint pStart;
    QWidget *wTarget{};
};

#endif // TITLE_BAR_H
