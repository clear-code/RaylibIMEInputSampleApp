# Copyright (C) 2022 Daijiro Fukuda <fukuda@clear-code.com>

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

project(RaylibIMEInputSampleApp C)

# TODO support MacOS.
# As or MacOS, there is a possibility to manage with xcode.
if(APPLE)
  message(FATAL_ERROR "Not support MacOS now.")
endif()

if(UNIX)
  find_package(raylib REQUIRED)
  find_package(Freetype REQUIRED)
endif()


add_library(CheckCharInput CheckCharInput.c)
list(APPEND LIBS CheckCharInput)

add_library(CFreeTypeManager CFreeTypeManager.c)
list(APPEND LIBS CFreeTypeManager)

add_executable(RaylibIMEInputSampleApp main.c)

set(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/")
list(APPEND DEPENDENCIES "./resources")


if(WIN32)
  set(RAYLIB "" CACHE STRING "The path of raylib binaries")
  if(NOT EXISTS ${RAYLIB})
    message(FATAL_ERROR "Not found raylib path: ${RAYLIB}. Must specify `RAYLIB` argument correctly.")
  endif()

  set(FREETYPE "" CACHE STRING "The path of freetype-windows-binaries")
  if(NOT EXISTS ${FREETYPE})
    message(FATAL_ERROR "Not found freetype path: ${FREETYPE}. Must specify `FREETYPE` argument correctly.")
  endif()

  set(RAYLIB_INCLUDE_DIR "${RAYLIB}/include")
  set(RAYLIB_LIBRARY "${RAYLIB}/lib/libraylib.a")
  set(FREETYPE_INCLUDE_DIR "${FREETYPE}/include")
  set(FREETYPE_LIBRARY "${FREETYPE}/release dll/win64/freetype.dll")

  target_include_directories(CheckCharInput PRIVATE ${RAYLIB_INCLUDE_DIR})
  target_link_libraries(CheckCharInput PRIVATE ${RAYLIB_LIBRARY})

  target_include_directories(CFreeTypeManager PUBLIC ${RAYLIB_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR})
  target_link_libraries(CFreeTypeManager PUBLIC ${RAYLIB_LIBRARY} ${FREETYPE_LIBRARY})

  target_include_directories(RaylibIMEInputSampleApp PRIVATE ${RAYLIB_INCLUDE_DIR})

  list(APPEND LIBS ${RAYLIB_LIBRARY})
  list(APPEND LIBS winmm imm32)
  list(APPEND DEPENDENCIES ${FREETYPE_LIBRARY})
endif()


if(UNIX)
  target_link_libraries(CheckCharInput PRIVATE raylib)

  target_include_directories(CFreeTypeManager PUBLIC ${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(CFreeTypeManager PUBLIC raylib freetype)

  list(APPEND LIBS raylib freetype)
  list(APPEND LIBS GL m pthread dl rt X11)
endif()


target_link_libraries(RaylibIMEInputSampleApp PRIVATE ${LIBS})


install(TARGETS RaylibIMEInputSampleApp DESTINATION ${INSTALL_PATH})


# copy files necessary to run the exe
foreach(path ${DEPENDENCIES})
  file(COPY ${path} DESTINATION ${INSTALL_PATH})
endforeach()
