#include "titleBar.h"

extern QString strFontBold;

TitleBar::TitleBar(QWidget *parent, int itemSize)
        : QWidget(parent) {
    if (itemSize == 0)
        itemSize = this->height();
    int itemSpace = itemSize / 4;
    int margin = itemSize / 6;
    itemSize -= margin * 2;
    QFont font(strFontBold, int(itemSize * 0.6));

    hLayout = new QHBoxLayout(this);
    hLayout->setSpacing(itemSpace);
    hLayout->setContentsMargins(margin, margin, margin, margin);

    lbIcon = new QLabel(this);
    lbIcon->setFixedSize(itemSize, itemSize);
    lbIcon->setPixmap(QPixmap(":/img/Icon-Blue.png"));
    lbIcon->setScaledContents(true);

    lbTitle = new QLabel(this);
    lbTitle->setFont(font);
    lbTitle->setScaledContents(true);

    hSpacer = new QSpacerItem(itemSize, itemSize, QSizePolicy::Expanding, QSizePolicy::Minimum);

    btnMinimize = new QPushButton(this);
    btnMinimize->setObjectName("MinimizeButton");
    btnMinimize->setFixedSize(itemSize, itemSize);

    btnClose = new QPushButton(this);
    btnClose->setObjectName("CloseButton");
    btnClose->setFixedSize(itemSize, itemSize);

    hLayout->addWidget(lbIcon);
    hLayout->addWidget(lbTitle);
    hLayout->addItem(hSpacer);
    hLayout->addWidget(btnMinimize);
    hLayout->addWidget(btnClose);
}

void TitleBar::setTarget(QWidget *target) {
    wTarget = target;

    connect(btnMinimize, &QPushButton::clicked, target, &QWidget::showMinimized);
    connect(btnClose, &QPushButton::clicked, target, &QWidget::close);
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        mPressed = true;
    if (wTarget) {
#if (QT_VERSION_MAJOR < 6)
        pStart = event->globalPos() - wTarget->pos();
#else
        pStart = event->globalPosition().toPoint() - wTarget->pos();
#endif
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *) {
    mPressed = false;
}

void TitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (mPressed && wTarget) {
#if (QT_VERSION_MAJOR < 6)
        QPoint pCur = event->globalPos();
#else
        QPoint pCur = event->globalPosition().toPoint();
#endif
        wTarget->move(pCur - pStart);
    }
}
