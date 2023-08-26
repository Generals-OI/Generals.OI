#ifndef END_WINDOW_H
#define END_WINDOW_H

#include <QWidget>
#include <QApplication>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class EndWindow; }
QT_END_NAMESPACE

class EndWindow : public QWidget {
Q_OBJECT

public:
    explicit EndWindow(QWidget *parent = nullptr);

    ~EndWindow() override;

    void updateText(const QString &strTitle, const QString &strContent = QString());

    void gameEnded();

signals:

    void watch();

protected:
    void onWatchButtonClicked();

private:
    Ui::EndWindow *ui;
};


#endif // END_WINDOW_H
