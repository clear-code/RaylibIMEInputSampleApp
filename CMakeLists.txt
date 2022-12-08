# Copyright (C) 2022 Daijiro Fukuda <fukuda@clear-code.com>
# Copyright (C) 2022 Takuro Ashie <ashie@clear-code.com>

# This file is part of RaylibIMEInputSampleApp.

# RaylibIMEInputSampleApp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# RaylibIMEInputSampleApp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.0)

project(RaylibIMEInputSampleApp VERSION 0.1.0 LANGUAGES C)


option(USE_EXTERNAL_GLFW "Use raylib built with external GLFW. GLFW package needs to be found." OFF)
option(USE_SOFT_FULLSCREEN "Use soft-fullscreen feature of raylib." OFF)

if (NOT WIN32)
    find_package(raylib REQUIRED)
    find_package(Freetype REQUIRED)
    if (USE_EXTERNAL_GLFW)
        find_package(glfw3 REQUIRED)
    endif()
endif()


add_library(PreeditManager PreeditManager.c)
list(APPEND LIBS PreeditManager)

add_library(TextEditor TextEditor.c)
list(APPEND LIBS TextEditor)

add_library(TextureManager TextureManager.c)
list(APPEND LIBS TextureManager)

add_executable(RaylibIMEInputSampleApp MACOSX_BUNDLE main.c)
target_include_directories(RaylibIMEInputSampleApp PRIVATE "./external/raygui")

set(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/")
set(RESOURCE_DIR ${INSTALL_PATH})

file(GLOB RESOUCE_FILES LIST_DIRECTORIES FALSE "./resources/*")
list(APPEND DEPENDENCIES ${RESOUCE_FILES})


if (WIN32)
    message("For Windows")

    option(MANAGE_PREEDIT_CANDIDATE "Manage the drawing of preedit candidates by the app side." OFF)

    set(RAYLIB "" CACHE STRING "The path of raylib binaries")
    if (NOT EXISTS ${RAYLIB})
        message(FATAL_ERROR "Not found raylib path: ${RAYLIB}. Must specify `RAYLIB` argument correctly.")
    endif()

    set(FREETYPE "" CACHE STRING "The path of freetype-windows-binaries")
    if (NOT EXISTS ${FREETYPE})
        message(FATAL_ERROR "Not found freetype path: ${FREETYPE}. Must specify `FREETYPE` argument correctly.")
    endif()

    set(GLFW "" CACHE STRING "The path of GLFW binaries. This is used only when `USE_EXTERNAL_GLFW` is TRUE.")
    if (USE_EXTERNAL_GLFW AND NOT EXISTS ${GLFW})
        message(FATAL_ERROR "Not found GLFW path: ${GLFW}. Must specify `GLFW` argument correctly.")
    endif()

    set(RAYLIB_INCLUDE_DIR "${RAYLIB}/include")
    set(RAYLIB_LIBRARY "${RAYLIB}/lib/libraylib.a")
    set(FREETYPE_INCLUDE_DIR "${FREETYPE}/include")
    set(FREETYPE_LIBRARY "${FREETYPE}/release dll/win64/freetype.dll")
    if (USE_EXTERNAL_GLFW)
        set(GLFW_INCLUDE_DIR "${GLFW}/include")
        set(GLFW_LIBRARY "${GLFW}/lib/libglfw3.a")
    endif()

    list(APPEND RAYLIBS ${RAYLIB_LIBRARY})
    if (USE_EXTERNAL_GLFW)
        list(APPEND RAYLIBS ${GLFW_LIBRARY})
    endif()

    target_include_directories(PreeditManager PRIVATE ${RAYLIB_INCLUDE_DIR})
    target_link_libraries(PreeditManager PRIVATE ${RAYLIBS})

    target_include_directories(TextureManager PUBLIC ${RAYLIB_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR})
    target_link_libraries(TextureManager PUBLIC ${RAYLIBS} ${FREETYPE_LIBRARY})

    target_include_directories(TextEditor PRIVATE ${RAYLIB_INCLUDE_DIR})
    target_link_libraries(TextEditor PRIVATE ${RAYLIBS} PreeditManager TextureManager)

    target_include_directories(RaylibIMEInputSampleApp PRIVATE ${RAYLIB_INCLUDE_DIR})

    list(APPEND LIBS ${RAYLIBS} winmm)

    list(APPEND DEPENDENCIES ${FREETYPE_LIBRARY})
elseif (APPLE)
    message("For MacOS")

    list(APPEND RAYLIBS raylib)
    if (USE_EXTERNAL_GLFW)
        list(APPEND RAYLIBS glfw)
    endif()

    target_link_libraries(PreeditManager PRIVATE ${RAYLIBS})

    target_include_directories(TextureManager PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(TextureManager PUBLIC ${RAYLIBS} ${FREETYPE_LIBRARIES})

    target_link_libraries(TextEditor PRIVATE ${RAYLIBS} PreeditManager TextureManager)

    list(APPEND LIBS
        ${RAYLIBS}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework OpenGL"
    )

    # Build as a macOS app bundle
    set(RESOURCE_DIR ${INSTALL_PATH}${CMAKE_PROJECT_NAME}.app/Contents/Resources/)
    set_target_properties(RaylibIMEInputSampleApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
    )
elseif (UNIX)
    message("For UNIX")

    list(APPEND RAYLIBS raylib)
    if (USE_EXTERNAL_GLFW)
        list(APPEND RAYLIBS glfw)
    endif()

    target_link_libraries(PreeditManager PRIVATE ${RAYLIBS})

    target_include_directories(TextureManager PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(TextureManager PUBLIC ${RAYLIBS} freetype)

    target_link_libraries(TextEditor PRIVATE ${RAYLIBS} PreeditManager TextureManager)

    list(APPEND LIBS ${RAYLIBS} GL m pthread dl rt X11)
endif()

# Copy DEPENDENCIES to INSTALL_PATH
# TODO: In macOS, somehow this copies DEPENDENCIES to both the bundle's Resources dir and directly under the install dir 
set_target_properties(RaylibIMEInputSampleApp PROPERTIES RESOURCE "${DEPENDENCIES}")

target_link_libraries(RaylibIMEInputSampleApp PRIVATE ${LIBS})
target_sources(RaylibIMEInputSampleApp PRIVATE ${DEPENDENCIES})

configure_file(config.h.in config.h)
target_include_directories(PreeditManager PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(TextureManager PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(TextEditor PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(RaylibIMEInputSampleApp PUBLIC ${PROJECT_BINARY_DIR})

install(TARGETS RaylibIMEInputSampleApp DESTINATION ${INSTALL_PATH})
