cmake_minimum_required(VERSION 3.16)
project(RawrXD_StubGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

qt_standard_project_setup()

qt_add_executable(RawrXD_StubGUI
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(RawrXD_StubGUI PRIVATE Qt6::Widgets)
