cmake_minimum_required(VERSION 3.5)

project(Generals.OI VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
        src
        src/endWindow
        src/gameWindow
        src/geometry
        src/globalMap
        src/playerInfo
        src/point
        src/server
        src/serverMap
        src/startWindow
        src/titleBar
        res
        res/aud
        res/font
        res/img
        res/qss
)

find_package(
        QT NAMES
        Qt5 Qt6
        REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        WebSockets
        Multimedia
)
find_package(
        Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        WebSockets
        Multimedia
)

set(
        PROJECT_SOURCES

        src/main.cpp

        src/gameWindow/focus.cpp
        src/gameWindow/gameWindow.cpp
        src/gameWindow/highlighter.cpp
        src/gameWindow/moveInfo.cpp
        src/gameWindow/gameWindow.h

        src/startWindow/startWindow.cpp
        src/startWindow/startWindow.h

        src/server/server.cpp
        src/server/server.h

        src/playerInfo/playerInfo.cpp
        src/playerInfo/playerInfo.h

        src/endWindow/endWindow.cpp
        src/endWindow/endWindow.h

        src/titleBar/titleBar.cpp
        src/titleBar/titleBar.h

        src/point/point.cpp
        src/point/point.h

        src/serverMap/generate.cpp
        src/serverMap/serverMap.cpp
        src/serverMap/serverMap.h

        src/globalMap/globalMap.cpp
        src/globalMap/globalMap.h

        src/geometry/geometry.cpp
        src/geometry/geometry.h
)
set(
        PROJECT_RESOURCES
        res/generals.qrc
        res/generals.rc
)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 5)
    add_executable(
            Generals.OI
            ${PROJECT_SOURCES}
            ${PROJECT_RESOURCES}
    )
endif ()

add_executable(
        Test-Generate

        src/test000.cpp

        src/point/point.cpp
        src/point/point.h

        src/serverMap/generate.cpp
        src/serverMap/serverMap.cpp
        src/serverMap/serverMap.h

        src/globalMap/globalMap.cpp
        src/globalMap/globalMap.h

        src/geometry/geometry.cpp
        src/geometry/geometry.h
)

target_link_libraries(
        Generals.OI PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::WebSockets
        Qt${QT_VERSION_MAJOR}::Multimedia
)

set_target_properties(
        Generals.OI PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
)

install(
        TARGETS Generals.OI
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
