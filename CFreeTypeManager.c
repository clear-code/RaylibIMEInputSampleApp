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

bool FreeTypeManager_Initialize(FreeTypeManager* ftManager, const char* fontFilepath, int width,
                               int height, int fontSize)
{
    setlocale(LC_CTYPE, "ja_JP");

    FreeTypeManager_SetTextureSize(ftManager, width, height);
    FreeTypeManager_SetFontSize(ftManager, fontSize);

    ftManager->m_FontSize = fontSize;

    //freetypeライブラリ初期化
    int error = FT_Init_FreeType(&ftManager->m_ftLibrary);
    if (error != 0)
    {
        printf("FT_Init_FreeType failed with error: %d\n", error);
        return false;
    }
    else
    {
        printf("FT_Init_FreeType success\n");
    }

    //読み込んだフォントからface作成
    error = FT_New_Face(ftManager->m_ftLibrary, fontFilepath, 0, &ftManager->m_ftFace);

    if (error != 0)
    {
        printf("FT_New_Face failed with error: %d\n", error);
        return false;
    }
    else
    {
        printf("FT_New_Face success\n");
    }

    //生成するグリフ用のフォント設定
    error = FT_Set_Char_Size(ftManager->m_ftFace, 0, 16 * ftManager->m_FontSize, 300, 300);

    if (error != 0)
    {
        printf("FT_Set_Char_Size failed with error: %d\n", error);
        return false;
    }
    else
    {
        printf("FT_Set_Char_Size success\n");
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

Texture2D FreeTypeManager_OutputRaylibImage(FreeTypeManager* ftManager, const int* view_text,
                                            int textLength, bool forPreedit)
{
    //画像データ作成用
    unsigned char* data = (unsigned char*)ftManager->m_Image.data;
    memcpy(data, ftManager->m_InitData, ftManager->m_TextureWidth * ftManager->m_TextureHeight * 4);

    FT_GlyphSlot slot;
    int startBufferIndex = 0;           //文字を書き始める
    int writeWidthPixelNum = 0;         //現在描画完了している横幅
    int writeHeightPixelNum = 0;

    //引数の文字の分だけグリフを取得して画像バッファに書き込む
    for (int glyfIndex = 0; glyfIndex < textLength; glyfIndex++ )
    {

        //文字のグリフを読み込み
        int error = FT_Load_Char(ftManager->m_ftFace, view_text[glyfIndex], FT_LOAD_RENDER);
        if (error != 0)
        {
            printf("FT_Load_Char failed with error: %d\n", error);
            continue;
        }
        slot = ftManager->m_ftFace->glyph;


        //描画する一文字を表示する高さの調整
        unsigned int hei = (ftManager->m_FontSize - slot->bitmap_top) * ftManager->m_TextureWidth * 4;
        //文字を表示する横幅の調整
        writeWidthPixelNum += slot->bitmap_left;

        //改行の判定
        if (writeWidthPixelNum + slot->bitmap.width >= ftManager->m_TextureWidth)
        {
            //描画スタート位置を次の行の先頭に
            writeWidthPixelNum = slot->bitmap_left;
            startBufferIndex += ftManager->m_FontSize * ftManager->m_TextureWidth * 4;
            writeHeightPixelNum += ftManager->m_FontSize;
        }

        //バッファにグリフの情報を書き込む
        for (int i = 0, wid = 0; i < slot->bitmap.width * slot->bitmap.rows; i++, wid += 4)
        {
            //文字の端まできたら書き込む位置を調整
            if (wid >= (slot->bitmap.width * 4))
            {
                wid = 0;
                hei = hei + ftManager->m_TextureWidth * 4;
            }

            //グリフ情報を書き込み
            unsigned char pix_color = 255 - slot->bitmap.buffer[i];
            if (pix_color != 255)
            {
                unsigned int writeIndex = startBufferIndex + hei + wid + (writeWidthPixelNum * 4);
                data[writeIndex] = pix_color;
                data[writeIndex + 1] = pix_color;
                data[writeIndex + 2] = pix_color;
                data[writeIndex + 3] = slot->bitmap.buffer[i];
            }
        }

        //次の文字の書き込み位置の計算
        writeWidthPixelNum += slot->bitmap.width;
    }

    if (!forPreedit)
    {
        ftManager->m_CursorPosX = writeWidthPixelNum;
        ftManager->m_CursorPosY = writeHeightPixelNum;
    }

    return LoadTextureFromImage(ftManager->m_Image);
}

void FreeTypeManager_SetFontSize(FreeTypeManager* ftManager, int fontSize)
{
    ftManager->m_FontSize = fontSize;
}

//テクスチャサイズ設定
void FreeTypeManager_SetTextureSize(FreeTypeManager* ftManager, int width, int height)
{
    ftManager->m_TextureWidth = width;
    ftManager->m_TextureHeight = height;
}
