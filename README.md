# Raylib IME Input Sample App

This is a sample application for Japanese input with [raylib](https://github.com/raysan5/raylib).

# How to build and run

## Windows

1. Install the following tools.
  - cmake: https://cmake.org/download/
  - mingw32-make: https://sourceforge.net/projects/mingw-w64/files/
2. Build raylib: https://github.com/raysan5/raylib

```console
$ git clone https://github.com/raysan5/raylib.git
$ cd raylib
$ cmake -B build -G "MinGW Makefiles" -D CMAKE_INSTALL_PREFIX=bin -D BUILD_EXAMPLES=OFF
$ mingw32-make -C build install
```

3. Get freetype binaries from https://github.com/ubawurinna/freetype-windows-binaries

```console
$ git clone https://github.com/ubawurinna/freetype-windows-binaries.git
```

4. Edit `build_windows.ps1`
  - Set the installed dir path of raylib to `$raylibBinDir`
  - Set the cloned dir path of freetype-windows-binaries to `$freeTypeWindowsBinDir`
5. Run `build_windows.ps1` by Windows PowerShell
6. Open `bin` dir newly created, and run `RaylibIMEInputSampleApp.exe`

## Ubuntu

TODO

## MacOS

TODO

# Licenses

## This Program

Copyright (C) 2022 鏡原 尚 <kagamihara@sena-networks.co.jp>
Copyright (C) 2022 Daijiro Fukuda <fukuda@clear-code.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## 源真ゴシック GenShinGothic-Regular.ttf

本ソフトでは表示フォントに「源真ゴシック」(http://jikasei.me/font/genshin/) を使用しています。  
Licensed under SIL Open Font License 1.1 (http://scripts.sil.org/OFL)  
© 2015 自家製フォント工房, © 2014, 2015 Adobe Systems Incorporated, © 2015 M+  
FONTS PROJECT
