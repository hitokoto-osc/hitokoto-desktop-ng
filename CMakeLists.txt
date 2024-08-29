cmake_minimum_required(VERSION 3.28)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(hitokoto-desktop-ng VERSION 0.1.0 LANGUAGES CXX)

include_directories(include)
add_executable(${PROJECT_NAME} main.cpp resources/systray.qrc
        src/hitokotowidget.cpp
        include/hitokotowidget.h
        src/hitokotowidget.ui
        include/conf.h
        src/conf.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Hitokoto Desktop NG")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE tinyxml2::tinyxml2)

set(${PROJECT_NAME}_resource_files "resources/logo.png" "resources/LXGWWenKaiMono-Regular.ttf")
qt_add_resources(${PROJECT_NAME} "${PROJECT_NAME}" PREFIX "/" FILES ${${PROJECT_NAME}_resource_files})
set_target_properties(${PROJECT_NAME} PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
qt_finalize_executable(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns"
        "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/icon.icns"
)
