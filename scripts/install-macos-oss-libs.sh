#!/bin/bash

# A very ad-hoc scripts to relocate prebuilt libfreetype & libpng (typically
# Homebrew's one) on macOS.
#
# Usage:
#   $ ./install-macos-oss-libs.sh /path/to/RaylibIMEInputSampleApp.app
#
# TODO:
# * Add license files.
# * Sign libfreetype & libpng, otherwise they don't work on Apple silicon.
#   Currently this script supports only x86_64 which doesn't require codesign.
# * Add install hook to CMakeLists.txt to run this script automatically.

APP_PATH=$1

if [ -z "${APP_PATH}" ]; then
    APP_PATH=RaylibIMEInputSampleApp.app
fi

APP_BIN_PATH="${APP_PATH}/Contents/MacOS/RaylibIMEInputSampleApp"
APP_DIR=$(dirname "$APP_BIN_PATH")

if [ ! -f "${APP_BIN_PATH}" ]; then
    echo "${APP_BIN_PATH} doesn't exist"
    exit 1
fi

otool -L "${APP_BIN_PATH}" | grep "@rpath/libfreetype.6.dylib" > /dev/null
if [ $? -eq 0 ]; then
    echo "rpath is already written in ${APP_BIN_PATH}, skip installation"
    exit 0
fi

FREETYPE_SRC=$(otool -L "${APP_BIN_PATH}"  | grep freetype | awk '{print $1}')
FREETYPE_NAME=$(basename "${FREETYPE_SRC}")
FREETYPE_RPATH=$(echo "${FREETYPE_SRC}" | sed -e "s#.*/${FREETYPE_NAME}#@rpath/${FREETYPE_NAME}#")

LIBPNG_SRC=$(otool -L "${FREETYPE_SRC}" | grep libpng | awk '{print $1}')
LIBPNG_NAME=$(basename "${LIBPNG_SRC}")
LIBPNG_RPATH=$(echo "${LIBPNG_SRC}" | sed -e "s#.*/${LIBPNG_NAME}#@rpath/${LIBPNG_NAME}#")

cp -f "${FREETYPE_SRC}" "${APP_DIR}" && \
cp -f "${LIBPNG_SRC}" "${APP_DIR}" && \
codesign --remove-signature "${APP_DIR}/${FREETYPE_NAME}" && \
codesign --remove-signature "${APP_DIR}/${LIBPNG_NAME}" && \
install_name_tool -id "${FREETYPE_RPATH}" "${APP_DIR}/${FREETYPE_NAME}" && \
install_name_tool -change "${LIBPNG_SRC}" "${LIBPNG_RPATH}" "${APP_DIR}/${FREETYPE_NAME}" && \
install_name_tool -id "${LIBPNG_RPATH}" "${APP_DIR}/${LIBPNG_NAME}" && \
install_name_tool -add_rpath @executable_path "${APP_BIN_PATH}" && \
install_name_tool -change "${FREETYPE_SRC}" "${FREETYPE_RPATH}" "${APP_BIN_PATH}"
