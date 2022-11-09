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

#include "CFreeTypeManager.h"

bool FreeTypeManager_Initialize(FreeTypeManager* ftManager, const char* fontFilepath, int width, int height, int fontSize)
{
    setlocale(LC_CTYPE, "ja_JP");

    ftManager->m_TextureWidth = width;
    ftManager->m_TextureHeight = height;
    ftManager->m_FontSize = fontSize;

    //freetypeライブラリ初期化
    int error = FT_Init_FreeType(&ftManager->m_ftLibrary);
    if (error != 0)
    {
        printf("FT_Init_FreeType failed with error: %d\n", error);
        return false;
    }

    //読み込んだフォントからface作成
    error = FT_New_Face(ftManager->m_ftLibrary, fontFilepath, 0, &ftManager->m_ftFace);
    if (error != 0)
    {
        printf("FT_New_Face failed with error: %d\n", error);
        return false;
    }

    //生成するグリフ用のフォント設定
    error = FT_Set_Char_Size(ftManager->m_ftFace, 0, 16 * ftManager->m_FontSize, 300, 300);
    if (error != 0)
    {
        printf("FT_Set_Char_Size failed with error: %d\n", error);
        return false;
    }

    unsigned int buffer_size = ftManager->m_TextureWidth * ftManager->m_TextureHeight * 4;

    //出力画像用の設定
    ftManager->m_Image.data = (unsigned char*)malloc(buffer_size);
    ftManager->m_Image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    ftManager->m_Image.mipmaps = 1;
    ftManager->m_Image.width = ftManager->m_TextureWidth;
    ftManager->m_Image.height = ftManager->m_TextureHeight;

    //出力画像用のバッファの初期化
    ftManager->m_InitData = (unsigned char*)malloc(buffer_size);
    for (int i = 0; i < buffer_size; i += 4)
    {
        ftManager->m_InitData[i] = 255;
        ftManager->m_InitData[i + 1] = 255;
        ftManager->m_InitData[i + 2] = 255;
        ftManager->m_InitData[i + 3] = 0;
    }

    return true;
}

Texture2D FreeTypeManager_OutputRaylibImage(FreeTypeManager* ftManager, const int* text, int textLength, bool forPreedit)
{
    // 画像データ初期化
    unsigned char* data = (unsigned char*)ftManager->m_Image.data;
    memcpy(data, ftManager->m_InitData, ftManager->m_TextureWidth * ftManager->m_TextureHeight * 4);

    // 現在何行目の描画をしているか
    int curRow = 0;
    // 現在の横方向描画位置(ピクセル)
    int curX = 0;

    // 1文字ずつ画像バッファに書き込む
    for (int glyfIndex = 0; glyfIndex < textLength; ++glyfIndex)
    {
        // グリフを読み込み
        int error = FT_Load_Char(ftManager->m_ftFace, text[glyfIndex], FT_LOAD_RENDER);
        if (error != 0)
        {
            printf("FT_Load_Char failed with error: %d\n", error);
            continue;
        }

        FT_GlyphSlot slot = ftManager->m_ftFace->glyph;

        // 文字を表示する横幅の調整
        curX += slot->bitmap_left;

        // テクスチャ幅をオーバーする場合は先に改行しておく
        if (curX + slot->bitmap.width >= ftManager->m_TextureWidth)
        {
            curRow++;
            //描画スタート位置を次の行の先頭に
            curX = slot->bitmap_left;
        }

        // この文字を書き込むべきオフセット位置
        unsigned int bufferOffset = 4 * (curX + curRow * ftManager->m_FontSize * ftManager->m_TextureWidth);

        // オフセットを起点として、この文字を書き込み処理を始める高さ
        // unsigned int glyfY = (ftManager->m_FontSize - slot->bitmap_top) * ftManager->m_TextureWidth * 4;
        int glyfY = ftManager->m_FontSize - slot->bitmap_top;

        // バッファにグリフの情報を書き込む
        for (int i = 0, glyfX = 0; i < slot->bitmap.width * slot->bitmap.rows; ++i, ++glyfX)
        {
            // 今の行(1文字の内部における1ピクセル行)の書き込みが終わったら、次の行へ
            if (glyfX >= slot->bitmap.width)
            {
                glyfX = 0;
                glyfY++;
            }

            // 1ピクセル書き込み
            unsigned char pixColor = 255 - slot->bitmap.buffer[i];
            if (pixColor != 255)
            {
                unsigned int writeIndex = bufferOffset + 4 * (glyfX + glyfY * ftManager->m_TextureWidth);
                data[writeIndex] = pixColor;
                data[writeIndex + 1] = pixColor;
                data[writeIndex + 2] = pixColor;
                data[writeIndex + 3] = slot->bitmap.buffer[i];
            }
        }

        // 次の文字の書き込み位置の計算
        curX += slot->bitmap.width;
    }

    if (!forPreedit)
    {
        ftManager->m_CursorPosX = curX;
        ftManager->m_CursorPosY = curRow * ftManager->m_FontSize;
    }

    return LoadTextureFromImage(ftManager->m_Image);
}
