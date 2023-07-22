#include "titleBar.h"

extern QString strFontBold;

TitleBar::TitleBar(QWidget *parent, int itemSize)
    : QWidget(parent) {
    if (itemSize == 0)
        itemSize = this->height();
    int itemSpace = itemSize / 2;
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
    hLayout->setContentsMargins(itemSpace, itemSpace, itemSpace, itemSpace);

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

void TitleBar::setParent(QWidget *parent) {
    wParent = parent;

    connect(btnMinimize, &QPushButton::clicked, parent, &QWidget::showMinimized);
    connect(btnClose, &QPushButton::clicked, parent, &QWidget::close);
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton)
        mPressed = true;
    if(wParent) {
        #if (QT_VERSION_MAJOR < 6)
            pStart = event->globalPos() - wParent->pos();
        #else
            pStart = event->globalPosition().toPoint() - wParent->pos();
        #endif
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *) {
    mPressed = false;
}

void TitleBar::mouseMoveEvent(QMouseEvent *event) {
    if(mPressed && wParent) {
        #if (QT_VERSION_MAJOR < 6)
            QPoint pCur = event->globalPos();
        #else
            QPoint pCur = event->globalPosition().toPoint();
        #endif
        wParent->move(pCur - pStart);
    }
}
