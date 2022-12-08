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

#ifndef TextureManager_h
#define TextureManager_h

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>

#include "config.h"
#include <ft2build.h>

#include FT_FREETYPE_H

typedef struct TextureManager
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

    // Use to create texture
    int m_currentX; // 現在何行目の描画をしているか
    int m_currentRow; // 現在の横方向描画位置(ピクセル)
} TextureManager;

bool TextureManager_Initialize(TextureManager* manager, const char* fontFilepath, int width, int height, int fontSize);

Texture2D TextureManager_CreateTexture(TextureManager* manager, const int* text, int textLength);

#ifdef MANAGE_PREEDIT_CANDIDATE
    Texture2D TextureManager_CreateTextureForCandidate(TextureManager* manager, int** candidates, int* lengths,
                                                       int pageSize, int selectedIndex);
#endif

#endif /* TextureManager_h */
