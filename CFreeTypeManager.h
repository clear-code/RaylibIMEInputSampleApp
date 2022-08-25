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

    // bitmapグリフ出力用の設定と渡す用のデータ
    Image m_ImageConf;

    // 画像バッファ（初期化用）
    unsigned char* m_InitData;

    // グリフ設定
    int m_FontSize;     // フォントサイズ

    // ウィンドウサイズ
    int m_TextureWidth;
    int m_TextureHeight;

    // カーソル位置
    int m_CursorPosX;
    int m_CursorPosY;
} FreeTypeManager;

//FreeTypeManagerの初期化
bool FreeTypeManager_Initialize(FreeTypeManager* ft_manager, char* font_filepath, int width,
                               int height, int font_size);

//渡した文字列からTexture2Dを作成する
Texture2D FreeTypeManager_OutputRaylibImage(FreeTypeManager* ft_manager, const unsigned int* view_text,
                                            unsigned int text_len, bool for_preedit);

//フォントサイズ設定
void FreeTypeManager_SetFontSize(FreeTypeManager* ft_manager, int font_size);

//テクスチャサイズ設定
void FreeTypeManager_SetTextureSize(FreeTypeManager* ft_manager, int width, int height);

#endif /* CFreeTypeManager_h */
