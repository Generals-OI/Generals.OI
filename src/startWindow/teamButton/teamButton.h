#ifndef TEAM_BUTTON_H
#define TEAM_BUTTON_H

#include <QPushButton>

class TeamButton : public QPushButton {
Q_OBJECT

public:
    explicit TeamButton(QWidget *parent, int idButton);

signals:

    void chosen(int idButton);

protected:
    void onButtonClicked();

private:
    int idButton;
};

#endif // TEAM_BUTTON_H
