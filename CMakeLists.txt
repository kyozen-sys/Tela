cmake_minimum_required(VERSION 3.25)

project(tela VERSION "0.1.0" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/backend/.*")

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/renderer/.*")

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ./include
    PRIVATE ./src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

find_package(PkgConfig REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PROJECT_LIBRARIES)

set(PROJECT_PLATFORM "linux" CACHE STRING "The platform to build for (linux|windows)")

set_property(CACHE PROJECT_PLATFORM PROPERTY STRINGS linux windows)

if(PROJECT_PLATFORM STREQUAL "linux")
    file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS src/platform/linux/*.cpp)

    target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM_SOURCES})
elseif(PROJECT_PLATFORM STREQUAL "windows")
    message(FATAL_ERROR "Windows backend is not yet supported.")
else()
    message(FATAL_ERROR "Unsupported PROJECT_PLATFORM='${PROJECT_PLATFORM}'. " "Valid values are: linux, windows")
endif()

set(PROJECT_BACKEND "wayland" CACHE STRING "The backend to build for (wayland|x11)")

set_property(CACHE PROJECT_BACKEND PROPERTY STRINGS wayland x11)

if(PROJECT_BACKEND STREQUAL "wayland")
    pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)

    file(GLOB_RECURSE BACKEND_SOURCES CONFIGURE_DEPENDS src/backend/wayland/*.cpp)

    target_sources(${PROJECT_NAME} PRIVATE ${BACKEND_SOURCES})

    include(WaylandProtocol)

    find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

    pkg_check_modules(LIBDECOR REQUIRED IMPORTED_TARGET libdecor-0)

    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

    pkg_get_variable(WL_PROTOCOLS_DIR wayland-protocols pkgdatadir)

    wayland_protocol(${PROJECT_NAME} ${WL_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)

    list(APPEND PROJECT_LIBRARIES PkgConfig::WAYLAND_CLIENT PkgConfig::LIBDECOR)
elseif(PROJECT_BACKEND STREQUAL "x11")
    message(FATAL_ERROR "X11 backend is not yet supported.")
else()
    message(FATAL_ERROR "Unsupported PROJECT_BACKEND='${PROJECT_BACKEND}'. " "Valid values are: wayland, x11")
endif()

set(PROJECT_RENDERER "opengl" CACHE STRING "The renderer to build for (opengl|vulkan)")

set_property(CACHE PROJECT_RENDERER PROPERTY STRINGS opengl vulkan)

if(PROJECT_RENDERER STREQUAL "opengl")
    pkg_check_modules(GL REQUIRED IMPORTED_TARGET gl)
    
    pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)

    pkg_check_modules(EPOXY REQUIRED IMPORTED_TARGET epoxy)

    if("${PROJECT_BACKEND}" STREQUAL "wayland")
        pkg_check_modules(WAYLAND_EGL REQUIRED IMPORTED_TARGET wayland-egl)

        list(APPEND PROJECT_LIBRARIES PkgConfig::WAYLAND_EGL)
    endif()

    file(GLOB_RECURSE RENDERER_SOURCES CONFIGURE_DEPENDS src/renderer/opengl/*.cpp)

    target_sources(${PROJECT_NAME} PRIVATE ${RENDERER_SOURCES})

    list(APPEND PROJECT_LIBRARIES PkgConfig::GL PkgConfig::EGL PkgConfig::EPOXY)
elseif (PROJECT_RENDERER STREQUAL "vulkan")
    message(FATAL_ERROR "Vulkan renderer is not yet supported.")
else()
    message(FATAL_ERROR "Unsupported PROJECT_RENDERER='${PROJECT_RENDERER}'. " "Valid values are: opengl, vulkan")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARIES})

include(FetchContent)

FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb GIT_TAG master)

FetchContent_MakeAvailable(stb)

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

option(BUILD_EXAMPLES "Build example applications" OFF)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()