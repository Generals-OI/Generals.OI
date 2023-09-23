cmake_minimum_required(VERSION 3.5)

project(Generals.OI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
        res
        res/aud
        res/font
        res/img
        res/qss
        src
        src/geometry
        src/point
        src/gameMap
        src/gameMap/basicMap
        src/gameMap/clientMap
        src/gameMap/serverMap
        src/gameMap/mapGenerator
        src/gameWindow/surrenderWindow
        src/gameWindow/endWindow
        src/gameWindow
        src/gameInformation
        src/server
        src/server/recorder
        src/startWindow
        src/startWindow/teamButton
        src/windowFrame/titleBar
        src/server/serverSettingsWindow
        src/windowFrame
        src/processJson
        src/replayWindow
        src/gameMapGrid/gameButton
        src/gameMapGrid
        src/gameMapEditor
)

find_package(
        QT NAMES
        Qt5
        Qt6
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

message("Current Qt version: " ${QT_VERSION})
message("Current build type: " ${CMAKE_BUILD_TYPE})

set(
        PROJECT_SOURCES

        src/main.cpp

        src/point/point.cpp
        src/point/point.h

        src/gameMap/serverMap/serverMap.cpp
        src/gameMap/serverMap/serverMap.h

        src/gameMap/clientMap/clientMap.cpp
        src/gameMap/clientMap/clientMap.h

        src/gameMap/basicMap/basicMap.cpp
        src/gameMap/basicMap/basicMap.h

        src/gameMap/mapGenerator/mapGenerator.cpp
        src/gameMap/mapGenerator/mapGenerator.h

        src/geometry/geometry.cpp
        src/geometry/geometry.h

        src/gameWindow/focus.cpp
        src/gameWindow/gameWindow.cpp
        src/gameWindow/boardLabel.cpp
        src/gameWindow/highlighter.cpp
        src/gameWindow/moveInfo.cpp
        src/gameWindow/gameWindow.h

        src/gameWindow/surrenderWindow/surrenderWindow.cpp
        src/gameWindow/surrenderWindow/surrenderWindow.h
        src/gameWindow/surrenderWindow/surrenderWindow.ui

        src/gameWindow/endWindow/endWindow.cpp
        src/gameWindow/endWindow/endWindow.h
        src/gameWindow/endWindow/endWindow.ui

        src/startWindow/startWindow.cpp
        src/startWindow/startWindow.h
        src/startWindow/startWindow.ui

        src/startWindow/teamButton/teamButton.cpp
        src/startWindow/teamButton/teamButton.h

        src/server/server.cpp
        src/server/server.h

        src/server/recorder/recorder.cpp
        src/server/recorder/recorder.h

        src/server/serverSettingsWindow/serverSettingsWindow.cpp
        src/server/serverSettingsWindow/serverSettingsWindow.h
        src/server/serverSettingsWindow/serverSettingsWindow.ui

        src/windowFrame/windowFrame.cpp
        src/windowFrame/windowFrame.h

        src/windowFrame/titleBar/titleBar.cpp
        src/windowFrame/titleBar/titleBar.h

        src/processJson/processJson.h

        src/gameInformation/gameInformation.cpp
        src/gameInformation/gameInformation.h

        src/replayWindow/replayWindow.cpp
        src/replayWindow/replayWindow.h
        src/replayWindow/controlPanel.ui

        src/gameMapGrid/gameMapGrid.cpp
        src/gameMapGrid/gameMapGrid.h

        src/gameMapGrid/gameButton/gameButton.cpp
        src/gameMapGrid/gameButton/gameButton.h

        src/gameMapEditor/gameMapEditor.cpp
        src/gameMapEditor/gameMapEditor.h
        src/gameMapEditor/gameMapEditor.ui
        src/gameMapEditor/sizeEditor.ui
)

set(
        PROJECT_RESOURCES
        res/generals.qrc
        res/generals.rc)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 5)
    add_executable(
            Generals.OI
            ${PROJECT_SOURCES}
            ${PROJECT_RESOURCES}
    )
endif ()

add_executable(
        Test-Generate

        src/test-generate.cpp

        src/point/point.cpp
        src/point/point.h

        src/gameMap/basicMap/basicMap.cpp
        src/gameMap/basicMap/basicMap.h

        src/gameMap/serverMap/serverMap.cpp
        src/gameMap/serverMap/serverMap.h

        src/gameMap/clientMap/clientMap.cpp
        src/gameMap/clientMap/clientMap.h

        src/gameMap/mapGenerator/mapGenerator.cpp
        src/gameMap/mapGenerator/mapGenerator.h

        src/geometry/geometry.cpp
        src/geometry/geometry.h
)

add_executable(
        Test-Recorder

        src/test-recorder.cpp

        src/server/recorder/recorder.cpp
        src/server/recorder/recorder.h

        src/point/point.cpp
        src/point/point.h

        src/gameMap/basicMap/basicMap.cpp
        src/gameMap/basicMap/basicMap.h

        src/gameMap/serverMap/serverMap.cpp
        src/gameMap/serverMap/serverMap.h

        src/gameMap/clientMap/clientMap.cpp
        src/gameMap/clientMap/clientMap.h

        src/gameMap/mapGenerator/mapGenerator.cpp
        src/gameMap/mapGenerator/mapGenerator.h

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

target_link_libraries(
        Test-Generate PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
)

target_link_libraries(
        Test-Recorder PRIVATE
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
