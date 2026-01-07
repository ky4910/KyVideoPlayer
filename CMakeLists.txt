cmake_minimum_required(VERSION 3.16)

project(KyVideoPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Qml)


set(FFMPEG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ffmpeg60)
set(SDL2_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2-2.30.10/x86_64-w64-mingw32)

include_directories(
    ${FFMPEG_ROOT}/include
    ${SDL2_ROOT}/include
)

link_directories(
    ${FFMPEG_ROOT}/lib
    ${SDL2_ROOT}/lib
)

add_definitions(-D__INTRIN_H_INCLUDED)

qt_policy(SET QTP0001 NEW)

qt_add_resources(RESOURCES qml.qrc shaders.qrc)
qt_add_executable(appKyVideoPlayer
    main.cpp
    Controller/playercontroller.cpp
    Decoder/audiodecoder.cpp
    Decoder/videodecoder.cpp
    Demux/demuxworker.cpp
    Render/videooutputfbo.cpp
    Render/videooutputrenderer.cpp
    PlaybackContext/playcontext.cpp
    PlaybackContext/playstate.cpp
    PlaybackContext/playstateqml.cpp
    Tools/packetqueue.cpp
    ${RESOURCES}
)

include_directories(
    Controller
    Decoder
    Demux
    Render
    PlaybackContext
    Tools
)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appKyVideoPlayer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appKyVideoPlayer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# target_link_libraries(appKyVideoPlayer
#     PRIVATE Qt6::Quick
# )

target_link_libraries(appKyVideoPlayer
    PRIVATE
        SDL2
        Qt6::Core
        Qt6::Quick
        avcodec
        avformat
        avutil
        swscale
        swresample
    )
target_link_libraries(appKyVideoPlayer PRIVATE Qt6::Core)
target_link_libraries(appKyVideoPlayer PRIVATE Qt6::Core)
target_link_libraries(appKyVideoPlayer PRIVATE Qt6::Core)
target_link_libraries(appKyVideoPlayer PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS appKyVideoPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
