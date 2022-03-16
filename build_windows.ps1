# Please overwrite these variables.
$raylibBinDir = "C:/{...}/raylib/bin"
$freeTypeWindowsBinDir = "C:/{...}/freetype-windows-binaries"

if (Test-Path .\build\) {
    rm -Recurse .\build\
}

if (Test-Path .\bin\) {
    rm -Recurse .\bin\
}

cmake -B build -G "MinGW Makefiles" -D CMAKE_INSTALL_PREFIX=bin -D RAYLIB="$raylibBinDir" -D FREETYPE="$freeTypeWindowsBinDir"
mingw32-make -C build install
