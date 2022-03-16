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

# TODO support Unix and MacOS.
# As or MacOS, there is a possibility to manage with xcode.
if(NOT WIN32)
  message(FATAL_ERROR "For only Windows now.")
endif()

# command line arguments
set(RAYLIB "" CACHE STRING "The path of raylib binaries")
set(FREETYPE "" CACHE STRING "The path of freetype-windows-binaries")

if(NOT EXISTS ${RAYLIB})
  message(FATAL_ERROR "Not found raylib path: ${RAYLIB}. Must specify `RAYLIB` argument correctly.")
endif()
if(NOT EXISTS ${FREETYPE})
  message(FATAL_ERROR "Not found freetype path: ${FREETYPE}. Must specify `FREETYPE` argument correctly.")
endif()

# local variables
set(RAYLIB_INCLUDE_DIR "${RAYLIB}/include")
set(RAYLIB_LIBRARY "${RAYLIB}/lib/libraylib.a")
set(FREETYPE_INCLUDE_DIR "${FREETYPE}/include")
set(FREETYPE_LIBRARY "${FREETYPE}/release dll/win64/freetype.dll")
set(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/")
list(APPEND DEPENDENCIES "./resources")
list(APPEND DEPENDENCIES ${FREETYPE_LIBRARY})

add_library(CheckCharInput CheckCharInput.c)
target_include_directories(CheckCharInput PRIVATE ${RAYLIB_INCLUDE_DIR})
list(APPEND LIBS CheckCharInput)

add_library(CFreeTypeManager CFreeTypeManager.c)
target_link_libraries(CFreeTypeManager PRIVATE ${FREETYPE_LIBRARY})
target_include_directories(CFreeTypeManager PRIVATE ${FREETYPE_INCLUDE_DIR} ${RAYLIB_INCLUDE_DIR})
list(APPEND LIBS CFreeTypeManager)

add_executable(RaylibIMEInputSampleApp main.c)

target_include_directories(RaylibIMEInputSampleApp PRIVATE ${FREETYPE_INCLUDE_DIR} ${RAYLIB_INCLUDE_DIR})
list(APPEND LIBS ${RAYLIB_LIBRARY})
list(APPEND LIBS winmm)
list(APPEND LIBS imm32)
target_link_libraries(RaylibIMEInputSampleApp PRIVATE ${LIBS})

install(TARGETS RaylibIMEInputSampleApp DESTINATION ${INSTALL_PATH})

# copy dlls necessary to run the exe
foreach(path ${DEPENDENCIES})
  file(COPY ${path} DESTINATION ${INSTALL_PATH})
endforeach()
