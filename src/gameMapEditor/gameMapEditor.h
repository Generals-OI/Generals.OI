#ifndef GAMEMAPEDITOR_H
#define GAMEMAPEDITOR_H

#include <QWidget>

#include "gameMapGrid.h"
#include "sizeEditor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameMapEditor; }
QT_END_NAMESPACE

class GameMapEditor : public QWidget
{
    Q_OBJECT

public:
    GameMapEditor(QWidget *parent = nullptr);
    ~GameMapEditor();

private:
    Ui::GameMapEditor *ui;
    int h{},w{};
    auto m[65][65]= new GameButton;
};

#endif // GAMEMAPEDITOR_H
