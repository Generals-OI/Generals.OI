#include "teamButton.h"

TeamButton::TeamButton(QWidget *parent, int idButton) : QPushButton(parent), idButton(idButton) {
    connect(this, &QPushButton::clicked, this, &TeamButton::onButtonClicked);
}

void TeamButton::onButtonClicked() {
    emit chosen(idButton);
}
