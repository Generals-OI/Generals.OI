#ifndef SURRENDER_WINDOW_H
#define SURRENDER_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SurrenderWindow; }
QT_END_NAMESPACE

class SurrenderWindow : public QWidget {
Q_OBJECT
public:
    explicit SurrenderWindow(QWidget *parent = nullptr);

    ~SurrenderWindow() override;

signals:

    void surrendered();

protected:
    void surrender();

private:
    Ui::SurrenderWindow *ui;
};


#endif // SURRENDER_WINDOW_H
