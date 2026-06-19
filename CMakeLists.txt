cmake_minimum_required(VERSION 3.25)

project(tela VERSION "0.1.0" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/platform/.*")

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/renderer/.*")

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ./include
    PRIVATE ./src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

set(PROJECT_PLATFORM "wayland" CACHE STRING "The platform to build for (wayland|x11)")

set_property(CACHE PROJECT_PLATFORM PROPERTY STRINGS wayland x11)

find_package(PkgConfig REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PROJECT_LIBRARIES)

if(PROJECT_PLATFORM STREQUAL "wayland")
    pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)

    file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS src/platform/wayland/*.cpp)

    target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM_SOURCES})

    list(APPEND PROJECT_LIBRARIES PkgConfig::WAYLAND_CLIENT)

    include(WaylandProtocol)

    find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

    pkg_get_variable(WL_PROTOCOLS_DIR wayland-protocols pkgdatadir)

    wayland_protocol(${PROJECT_NAME} ${WL_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)
elseif (PROJECT_PLATFORM STREQUAL "x11")
    message(FATAL_ERROR "X11 platform is not yet supported.")
else()
    message(FATAL_ERROR "Unsupported PROJECT_PLATFORM='${PROJECT_PLATFORM}'. " "Valid values are: wayland, x11")
endif()

set(PROJECT_RENDERER "opengl" CACHE STRING "The renderer to build for (opengl|vulkan)")

set_property(CACHE PROJECT_RENDERER PROPERTY STRINGS opengl vulkan)

if(PROJECT_RENDERER STREQUAL "opengl")
    pkg_check_modules(GL REQUIRED IMPORTED_TARGET gl)
    
    pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)

    if("${PROJECT_PLATFORM}" STREQUAL "wayland")
        pkg_check_modules(WAYLAND_EGL REQUIRED IMPORTED_TARGET wayland-egl)

        list(APPEND PROJECT_LIBRARIES PkgConfig::WAYLAND_EGL)
    endif()

    file(GLOB_RECURSE RENDERER_SOURCES CONFIGURE_DEPENDS src/renderer/opengl/*.cpp)

    target_sources(${PROJECT_NAME} PRIVATE ${RENDERER_SOURCES})

    list(APPEND PROJECT_LIBRARIES PkgConfig::GL PkgConfig::EGL)
elseif (PROJECT_RENDERER STREQUAL "vulkan")
    message(FATAL_ERROR "Vulkan renderer is not yet supported.")
else()
    message(FATAL_ERROR "Unsupported PROJECT_RENDERER='${PROJECT_RENDERER}'. " "Valid values are: opengl, vulkan")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARIES})

option(BUILD_EXAMPLES "Build example applications" OFF)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()