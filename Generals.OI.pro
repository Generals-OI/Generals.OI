QT += core gui websockets widgets multimedia

CONFIG += c++11 utf8_source

INCLUDEPATH += \
    src \
    src/endWindow \
    src/gameWindow \
    src/geometry \
    src/globalMap \
    src/playerInfo \
    src/point \
    src/server \
    src/serverMap \
    src/startWindow \
    src/titleBar \
    res \
    res/aud \
    res/font \
    res/img \
    res/qss \

SOURCES += \
    src/main.cpp \
    src/gameWindow/focus.cpp \
    src/gameWindow/gameWindow.cpp \
    src/gameWindow/highlighter.cpp \
    src/gameWindow/moveInfo.cpp \
    src/startWindow/startWindow.cpp \
    src/server/server.cpp \
    src/playerInfo/playerInfo.cpp \
    src/endWindow/endWindow.cpp \
    src/titleBar/titleBar.cpp \
    src/point/point.cpp \
    src/serverMap/generate.cpp \
    src/serverMap/serverMap.cpp \
    src/globalMap/globalMap.cpp \
    src/geometry/geometry.cpp

HEADERS += \
    src/gameWindow/gameWindow.h \
    src/startWindow/startWindow.h \
    src/server/server.h \
    src/playerInfo/playerInfo.h \
    src/endWindow/endWindow.h \
    src/titleBar/titleBar.h \
    src/point/point.h \
    src/serverMap/serverMap.h \
    src/globalMap/globalMap.h \
    src/geometry/geometry.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/generals.qrc

RC_FILE += \
    res/generals.rc
