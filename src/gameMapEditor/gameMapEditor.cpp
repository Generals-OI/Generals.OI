#include "gameMapEditor.h"
#include "ui_gameMapEditor.h"

GameMapEditor::GameMapEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameMapEditor)
{
    ui->setupUi(this);
}

GameMapEditor::~GameMapEditor()
{
    delete ui;
}
