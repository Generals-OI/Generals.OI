#include "titleBar.h"

extern QString strFontBold;

TitleBar::TitleBar(QWidget *parent, int itemSize)
        : QWidget(parent) {
    if (itemSize == 0)
        itemSize = this->height();
    int itemSpace = itemSize / 2;
    int marginSize = itemSize / 4;
    itemSize /= 2;
    QFont font(strFontBold, int(itemSize * 0.6));

    QFile cssFile(":/qss/WindowWidgets.qss");
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(cssFile.readAll());
        cssFile.close();
    }

    vLayout = new QVBoxLayout(this);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);

    fBorder = new QFrame(this);
    fBorder->setFrameShape(QFrame::StyledPanel);
    fBorder->setFrameShadow(QFrame::Raised);

    hLayout = new QHBoxLayout(fBorder);
    hLayout->setSpacing(itemSpace);
    hLayout->setContentsMargins(marginSize, marginSize, marginSize, marginSize);

    lbIcon = new QLabel(fBorder);
    lbIcon->setFixedSize(itemSize, itemSize);
    lbIcon->setPixmap(QPixmap(":/img/Icon.png"));
    lbIcon->setScaledContents(true);

    lbTitle = new QLabel(fBorder);
    lbTitle->setFont(font);
    lbTitle->setScaledContents(true);

    hSpacer = new QSpacerItem(itemSize, itemSize, QSizePolicy::Expanding, QSizePolicy::Minimum);

    btnMinimize = new QPushButton(fBorder);
    btnMinimize->setObjectName("MinimizeButton");
    btnMinimize->setFixedSize(itemSize, itemSize);

    btnClose = new QPushButton(fBorder);
    btnClose->setObjectName("CloseButton");
    btnClose->setFixedSize(itemSize, itemSize);

    hLayout->addWidget(lbIcon);
    hLayout->addWidget(lbTitle);
    hLayout->addItem(hSpacer);
    hLayout->addWidget(btnMinimize);
    hLayout->addWidget(btnClose);

    vLayout->addWidget(fBorder);
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
