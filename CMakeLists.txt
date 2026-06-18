cmake_minimum_required(VERSION 3.25)

project(tela VERSION "0.1.0" LANGUAGES C CXX)

option(BUILD_EXAMPLES "Build example applications" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_PLATFORM "wayland" CACHE STRING "The platform to build for (e.g. wayland, x11, etc.)")

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

file(GLOB_RECURSE PROJECT_PLATFORM_SOURCES CONFIGURE_DEPENDS src/platform/${PROJECT_PLATFORM}/*.cpp)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/platform/.*")

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES} ${PROJECT_PLATFORM_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ./include
    PRIVATE ./src
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)

pkg_check_modules(GL REQUIRED IMPORTED_TARGET gl)
pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)

target_link_libraries(${PROJECT_NAME}
    PkgConfig::WAYLAND_CLIENT
    PkgConfig::GL
    PkgConfig::EGL
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(WaylandProtocol)

find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

pkg_get_variable(WL_PROTOCOLS_DIR wayland-protocols pkgdatadir)

wayland_protocol(${PROJECT_NAME} ${WL_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()