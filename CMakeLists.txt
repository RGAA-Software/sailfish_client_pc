cmake_minimum_required(VERSION 3.25)

include(env.cmake)

#if(WIN32)
##    set(VCPKG_ROOT C:/source/vcpkg)
##    set(QT_ROOT C:/Qt/Qt6.4/6.4.0/msvc2019_64)
#
#    set(VCPKG_ROOT G:/vcpkg)
#    set(QT_ROOT F:/qt6.4/6.4.0/msvc2019_64)
#
#    set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
#elseif (UNIX)
#
#endif ()

set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
SET(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
#include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(sailfish_client_pc)

include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# config for each platform
if(WIN32)
    message("Config for Windows")

    add_definitions(-DOS_WINDOWS)
    add_compile_options(/bigobj)

elseif(APPLE)
    message("Config for Apple")

    find_package(FFMPEG REQUIRED)
    set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations ")

endif ()
# config end

# find packages
find_package(SqliteOrm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(libyuv CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets Multimedia)
find_package(Boost COMPONENTS system filesystem thread locale program_options random)
find_package(leveldb CONFIG REQUIRED)
# find packages end

# include
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/src/sdk)
include_directories(${QT_ROOT}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/deps)
include_directories(${CMAKE_SOURCE_DIR}/deps/websocketpp)
include_directories(${CMAKE_SOURCE_DIR}/deps/rgaa_common)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
    MESSAGE(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
    MESSAGE(STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
endif()
# include end

add_subdirectory(src)
add_subdirectory(deps)

set(RES resources/font.qrc resources/main.qrc resources/qss.qrc resources.qrc)

add_executable(sailfish_client_pc main.cpp logo.rc ${RES})
target_link_libraries(sailfish_client_pc main sdk ui settings common opus_codec)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(TARGET sailfish_client_pc PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/lib/avcodec.lib"  "${CMAKE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/lib/avutil.lib"  "${CMAKE_BINARY_DIR}"
    )
    add_custom_command(TARGET sailfish_client_pc POST_BUILD
        COMMAND ${QT_ROOT}/bin/windeployqt.exe $<TARGET_FILE:${PROJECT_NAME}> --no-translations
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/bin/avcodec-60.dll"  "${CMAKE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/bin/avutil-58.dll"  "${CMAKE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/bin/swresample-4.dll"  "${CMAKE_BINARY_DIR}"
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Config for Linux")

    add_definitions(-DOS_LINUX)

endif ()