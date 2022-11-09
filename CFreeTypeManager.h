// Copyright (C) 2022 鏡原 尚 <kagamihara@sena-networks.co.jp>

// This file is part of RaylibIMEInputSampleApp.

// RaylibIMEInputSampleApp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// RaylibIMEInputSampleApp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CFreeTypeManager_h
#define CFreeTypeManager_h

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>

#include <ft2build.h>

#include FT_FREETYPE_H

typedef struct FreeTypeManager
{
    FT_Library m_ftLibrary;
    FT_Face m_ftFace;

    // raylib Image
    Image m_Image;

    // buffer data to initialize
    unsigned char* m_InitData;

    int m_FontSize;

    int m_TextureWidth;
    int m_TextureHeight;

    // Currently, the cursor is always at the end of the text.
    int m_CursorPosX;
    int m_CursorPosY;
} FreeTypeManager;

bool FreeTypeManager_Initialize(FreeTypeManager* ftManager, const char* fontFilepath, int width, int height, int fontSize);

Texture2D FreeTypeManager_OutputRaylibImage(FreeTypeManager* ftManager, const int* text, int textLength, bool forPreedit);

#endif /* CFreeTypeManager_h */
