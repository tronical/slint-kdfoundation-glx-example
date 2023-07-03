cmake_minimum_required(VERSION 3.21)
project(my_application LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/ECM/find-modules")

include(FetchContent)

set(KDUTILS_BUILD_TESTS OFF)
if(EXISTS "${CMAKE_SOURCE_DIR}/kdutils")
    add_subdirectory(${CMAKE_SOURCE_DIR}/kdutils/)
else()
    find_package(KDUtils CONFIG)

    if(NOT KDUtils_FOUND)
        FetchContent_Declare(
            KDUtils
            GIT_REPOSITORY https://github.com/kdab/kdutils
            GIT_TAG master
            USES_TERMINAL_DOWNLOAD YES USES_TERMINAL_UPDATE YES
        )

        option(KDUTILS_BUILD_TESTS "Build the tests" OFF)

        FetchContent_MakeAvailable(KDUtils)
    endif()

    find_package(KDFoundation CONFIG)
    find_package(KDGui CONFIG)
endif()

add_compile_definitions(SLINT_FEATURE_EXPERIMENTAL)
set(SLINT_FEATURE_EXPERIMENTAL ON)
set(SLINT_FEATURE_BACKEND_QT OFF)
set(SLINT_FEATURE_RENDERER_WINIT_SKIA ON)
set(SLINT_FEATURE_RENDERER_WINIT_SKIA_OPENGL ON)
set(SLINT_FEATURE_RENDERER_WINIT_SKIA_VULKAN OFF)
# add_compile_definitions(KD_PLATFORM_WAYLAND)

find_package(Slint QUIET)
if (NOT Slint_FOUND)
  message("Slint could not be located in the CMake module search path. Downloading it from Git and building it locally")
  FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    # `release/1` will auto-upgrade to the latest Slint >= 1.0.0 and < 2.0.0
    # `release/1.0` will auto-upgrade to the latest Slint >= 1.0.0 and < 1.1.0
    GIT_TAG 08e20c858665975f3514eb0bb6defe0686d69692
    SOURCE_SUBDIR api/cpp
  )
  FetchContent_MakeAvailable(Slint)
endif (NOT Slint_FOUND)

set(SOURCES
    src/main.cpp
    src/window_adapter.cpp
)

set(LIBRARIES
    Slint::Slint
    KDUtils::KDUtils
    KDUtils::KDFoundation
    KDUtils::KDGui
)

# we need xcb symbols in order to get all the information necessary to create
# a slint window handle
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_package(
        XCB
        COMPONENTS XKB
        REQUIRED
    )
    list(APPEND LIBRARIES
        ${XCB_LIBRARIES}
        ${XKB_LIBRARIES}
    )
endif()

add_executable(my_application ${SOURCES})

slint_target_sources(my_application
    ui/appwindow.slint
)

target_link_libraries(my_application PRIVATE ${LIBRARIES})
include_directories(serial/include ${CURL_INCLUDE_DIRS} ${spdlog_INCLUDE_DIRS} ${KDUtils_INCLUDE_DIRS})
# On Windows, copy the Slint DLL next to the application binary so that it's found.
if (WIN32)
    add_custom_command(TARGET my_application POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:my_application> $<TARGET_FILE_DIR:my_application> COMMAND_EXPAND_LISTS)
endif()
