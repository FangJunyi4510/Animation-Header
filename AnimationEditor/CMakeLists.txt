cmake_minimum_required(VERSION 3.19)
project(AnimationEditor LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()


add_subdirectory(
    ../FFmpegEntity
    build/FFmpegEntity
)

qt_add_executable(AnimationEditor
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(AnimationEditor
    PRIVATE
        Qt::Core
        Qt::Widgets
        FFmpegEntity
)

include(GNUInstallDirs)

install(TARGETS AnimationEditor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET AnimationEditor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
