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
        src/geometry
        src/point
        src/gameMap
        src/gameMap/globalMap
        src/gameMap/serverMap
        src/gameMap/mapGenerator
        src/gameWindow/surrenderWindow
        src/gameWindow/endWindow
        src/gameWindow
        src/gameWindow/gameButton
        src/gameInformation
        src/server
        src/startWindow
        src/startWindow/teamButton
        src/windowFrame/titleBar
        src/server/serverSettingsWindow
        src/windowFrame
        src/processJson
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
        src/gameWindow/boardLabel.cpp
        src/gameWindow/highlighter.cpp
        src/gameWindow/moveInfo.cpp
        src/gameWindow/gameWindow.h

        src/gameWindow/surrenderWindow/surrenderWindow.cpp
        src/gameWindow/surrenderWindow/surrenderWindow.h
        src/gameWindow/surrenderWindow/surrenderWindow.ui

        src/startWindow/startWindow.cpp
        src/startWindow/startWindow.h
        src/startWindow/startWindow.ui

        src/startWindow/teamButton/teamButton.cpp
        src/startWindow/teamButton/teamButton.h

        src/server/server.cpp
        src/server/server.h

        src/processJson/processJson.h

        src/gameInformation/gameInformation.cpp
        src/gameInformation/gameInformation.h

        src/gameWindow/endWindow/endWindow.cpp
        src/gameWindow/endWindow/endWindow.h

        src/server/serverSettingsWindow/serverSettingsWindow.cpp
        src/server/serverSettingsWindow/serverSettingsWindow.h
        src/server/serverSettingsWindow/serverSettingsWindow.ui

        src/windowFrame/titleBar/titleBar.cpp
        src/windowFrame/titleBar/titleBar.h

        src/point/point.cpp
        src/point/point.h

        src/gameMap/serverMap/serverMap.cpp
        src/gameMap/serverMap/serverMap.h

        src/gameMap/globalMap/globalMap.cpp
        src/gameMap/globalMap/globalMap.h

        src/gameMap/mapGenerator/mapGenerator.cpp
        src/gameMap/mapGenerator/mapGenerator.h

        src/geometry/geometry.cpp
        src/geometry/geometry.h

        src/gameWindow/gameButton/gameButton.cpp
        src/gameWindow/gameButton/gameButton.h

        src/windowFrame/windowFrame.cpp
        src/windowFrame/windowFrame.h
)

set(
        PROJECT_RESOURCES
        res/generals.qrc
        res/generals.rc)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 5)
    add_executable(
            Generals.OI
            ${PROJECT_SOURCES}
            ${PROJECT_RESOURCES})
endif ()

add_executable(
        Test-Generate

        src/test-generate.cpp

        src/point/point.cpp
        src/point/point.h

        src/gameMap/serverMap/serverMap.cpp
        src/gameMap/serverMap/serverMap.h

        src/gameMap/globalMap/globalMap.cpp
        src/gameMap/globalMap/globalMap.h

        src/gameMap/mapGenerator/mapGenerator.cpp
        src/gameMap/mapGenerator/mapGenerator.h

        src/geometry/geometry.cpp
        src/geometry/geometry.h)

target_link_libraries(
        Generals.OI PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::WebSockets
        Qt${QT_VERSION_MAJOR}::Multimedia
)

target_link_libraries(
        Test-Generate PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
)

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set_target_properties(
            Generals.OI PROPERTIES
            WIN32_EXECUTABLE TRUE
            MACOSX_BUNDLE TRUE
    )
endif ()

install(
        TARGETS Generals.OI
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
