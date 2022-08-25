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

bool FreeTypeManager_Initialize(FreeTypeManager* ft_manager, char* font_filepath, int width,
                               int height, int font_size)
{
    setlocale(LC_CTYPE, "ja_JP");

    FreeTypeManager_SetTextureSize(ft_manager, width, height);
    FreeTypeManager_SetFontSize(ft_manager, font_size);

    ft_manager->m_FontSize = font_size;

    //freetypeライブラリ初期化
    int error = FT_Init_FreeType(&ft_manager->m_ftLibrary);
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
    error = FT_New_Face(ft_manager->m_ftLibrary, font_filepath, 0, &ft_manager->m_ftFace);

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
    error = FT_Set_Char_Size(ft_manager->m_ftFace, 0, 16 * ft_manager->m_FontSize, 300, 300);

    if (error != 0)
    {
        printf("FT_Set_Char_Size failed with error: %d\n", error);
        return false;
    }
    else
    {
        printf("FT_Set_Char_Size success\n");
    }

    unsigned int buffer_size = ft_manager->m_TextureWidth * ft_manager->m_TextureHeight * 4;

    //出力画像用の設定
    ft_manager->m_ImageConf.data = (unsigned char*)malloc(buffer_size);
    ft_manager->m_ImageConf.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    ft_manager->m_ImageConf.mipmaps = 1;
    ft_manager->m_ImageConf.width = ft_manager->m_TextureWidth;
    ft_manager->m_ImageConf.height = ft_manager->m_TextureHeight;

    //出力画像用のバッファの初期化
    ft_manager->m_InitData = (unsigned char*)malloc(buffer_size);
    for (int i = 0; i < buffer_size; i += 4)
    {
        ft_manager->m_InitData[i] = 255;
        ft_manager->m_InitData[i + 1] = 255;
        ft_manager->m_InitData[i + 2] = 255;
        ft_manager->m_InitData[i + 3] = 0;
    }

    return true;
}

Texture2D FreeTypeManager_OutputRaylibImage(FreeTypeManager* ft_manager, const unsigned int* view_text,
                                            unsigned int text_len, bool for_preedit)
{
    //画像データ作成用
    unsigned char* data = (unsigned char*)ft_manager->m_ImageConf.data;
    memcpy(data, ft_manager->m_InitData, ft_manager->m_TextureWidth * ft_manager->m_TextureHeight * 4);

    int error;
    FT_GlyphSlot slot;
    int startBufferIndex = 0;           //文字を書き始める
    int writeWidthPixelNum = 0;         //現在描画完了している横幅
    int writeHeightPixelNum = 0;

    //引数の文字の分だけグリフを取得して画像バッファに書き込む
    for (int glyfIndex = 0; glyfIndex < text_len; glyfIndex++ )
    {

        //文字のグリフを読み込み
        error = FT_Load_Char(ft_manager->m_ftFace, view_text[glyfIndex], FT_LOAD_RENDER);
        if (error != 0)
        {
            //printf("%d", error);
        }
        else
        {
            //printf("FT_load_Char success¥n");
        }
        slot = ft_manager->m_ftFace->glyph;


        //描画する一文字を表示する高さの調整
        unsigned int hei = (ft_manager->m_FontSize - slot->bitmap_top) * ft_manager->m_TextureWidth * 4;
        //文字を表示する横幅の調整
        writeWidthPixelNum += slot->bitmap_left;

        //改行の判定
        if (writeWidthPixelNum + slot->bitmap.width >= ft_manager->m_TextureWidth)
        {
            //描画スタート位置を次の行の先頭に
            writeWidthPixelNum = slot->bitmap_left;
            startBufferIndex += ft_manager->m_FontSize * ft_manager->m_TextureWidth * 4;
            writeHeightPixelNum += ft_manager->m_FontSize;
        }

        //バッファにグリフの情報を書き込む
        for (int i = 0, wid = 0; i < slot->bitmap.width * slot->bitmap.rows; i++, wid += 4)
        {
            //文字の端まできたら書き込む位置を調整
            if (wid >= (slot->bitmap.width * 4))
            {
                wid = 0;
                hei = hei + ft_manager->m_TextureWidth * 4;
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

    if (!for_preedit)
    {
        ft_manager->m_CursorPosX = writeWidthPixelNum;
        ft_manager->m_CursorPosY = writeHeightPixelNum;
    }

    return LoadTextureFromImage(ft_manager->m_ImageConf);
}

void FreeTypeManager_SetFontSize(FreeTypeManager* ft_manager, int font_size)
{
    ft_manager->m_FontSize = font_size;
}

//テクスチャサイズ設定
void FreeTypeManager_SetTextureSize(FreeTypeManager* ft_manager, int width, int height)
{
    ft_manager->m_TextureWidth = width;
    ft_manager->m_TextureHeight = height;
}
