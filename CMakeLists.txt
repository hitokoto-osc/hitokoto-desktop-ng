cmake_minimum_required(VERSION 3.28)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(hitokoto-desktop-ng LANGUAGES CXX)

include_directories(include)
add_executable(${PROJECT_NAME} main.cpp resources/systray.qrc
        src/hitokotowidget.cpp
        include/hitokotowidget.h
        src/hitokotowidget.ui
        include/conf.h
        src/conf.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE tinyxml2::tinyxml2)

set(${PROJECT_NAME}_resource_files "resources/logo.png" "resources/LXGWWenKaiMono-Regular.ttf")
qt_add_resources(${PROJECT_NAME} "${PROJECT_NAME}" PREFIX "/" FILES ${${PROJECT_NAME}_resource_files})
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
