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

#include "TextureManager.h"

#if defined(WIN32)
#include <libloaderapi.h>
#elif defined(APPLE)
#include <mach-o/dyld.h>
#endif

#define FONT_FILEPATH_MAX_LEN 256
#define FILEPATH_BUFFER_LEN 200 // Need to make this smaller than FONT_FILEPATH_MAX_LEN to suppress warning...

static unsigned int NumOfPixelsInOneRow(TextureManager* manager);
static void WriteOneChar(TextureManager* manager, unsigned char* data, int character, bool wordWrap);
static char* GetFontFilepath();

bool TextureManager_Initialize(TextureManager* manager, int width, int height, int fontSize)
{
    setlocale(LC_CTYPE, "ja_JP");

    manager->m_TextureWidth = width;
    manager->m_TextureHeight = height;
    manager->m_FontSize = fontSize;

    //freetypeライブラリ初期化
    int error = FT_Init_FreeType(&manager->m_ftLibrary);
    if (error != 0)
    {
        printf("FT_Init_FreeType failed with error: %d\n", error);
        return false;
    }

    const char* fontFilepath = GetFontFilepath();
    if (!fontFilepath)
    {
        printf("GetFontFilepath failed\n");
        return false;
    }

    //読み込んだフォントからface作成
    error = FT_New_Face(manager->m_ftLibrary, fontFilepath, 0, &manager->m_ftFace);
    if (error != 0)
    {
        printf("FT_New_Face failed with error: %d, fontFilepath: %s\n", error, fontFilepath);
        return false;
    }

    //生成するグリフ用のフォント設定
    error = FT_Set_Char_Size(manager->m_ftFace, 0, 16 * manager->m_FontSize, 300, 300);
    if (error != 0)
    {
        printf("FT_Set_Char_Size failed with error: %d\n", error);
        return false;
    }

    unsigned int bufferSize = manager->m_TextureWidth * manager->m_TextureHeight * 4;

    //出力画像用の設定
    manager->m_Image.data = (unsigned char*)malloc(bufferSize);
    manager->m_Image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    manager->m_Image.mipmaps = 1;
    manager->m_Image.width = manager->m_TextureWidth;
    manager->m_Image.height = manager->m_TextureHeight;

    //出力画像用のバッファの初期化
    manager->m_InitData = (unsigned char*)malloc(bufferSize);
    for (int i = 0; i < bufferSize; i += 4)
    {
        manager->m_InitData[i] = 255;
        manager->m_InitData[i + 1] = 255;
        manager->m_InitData[i + 2] = 255;
        manager->m_InitData[i + 3] = 0;
    }

    return true;
}

Texture2D TextureManager_CreateTexture(TextureManager* manager, const int* text, int textLength)
{
    // 画像データ初期化
    unsigned char* data = (unsigned char*)manager->m_Image.data;
    memcpy(data, manager->m_InitData, manager->m_TextureWidth * manager->m_TextureHeight * 4);

    manager->m_currentX = 0;
    manager->m_currentRow = 0;

    // 1文字ずつ画像バッファに書き込む
    for (int i = 0; i < textLength; ++i)
        WriteOneChar(manager, data, text[i], true);

    // TODO カーソル位置を動かせるようにする場合は、ここの計算処理も改良する
    manager->m_CursorPosX = manager->m_currentX;
    manager->m_CursorPosY = manager->m_currentRow * manager->m_FontSize;

    return LoadTextureFromImage(manager->m_Image);
}

#ifdef MANAGE_PREEDIT_CANDIDATE
    Texture2D TextureManager_CreateTextureForCandidate(TextureManager* manager, int** candidates, int* lengths,
                                                       int pageSize, int selectedIndex)
    {
        // 画像データ初期化
        unsigned char* data = (unsigned char*)manager->m_Image.data;
        memcpy(data, manager->m_InitData, manager->m_TextureWidth * manager->m_TextureHeight * 4);

        manager->m_currentX = 0;
        manager->m_currentRow = 0;

        for (int index = 0; index < pageSize; ++index)
        {
            if (index == selectedIndex)
            {
                WriteOneChar(manager, data, '>', false);
                WriteOneChar(manager, data, '>', false);
            }

            const int* text = candidates[index];
            for (int i = 0; i < lengths[index]; ++i)
                WriteOneChar(manager, data, text[i], false);

            manager->m_currentRow++;
            manager->m_currentX = 0;
        }

        // 使い道を想定していないが、とりあえず終端の位置にしておく
        manager->m_CursorPosX = manager->m_currentX;
        manager->m_CursorPosY = manager->m_currentRow * manager->m_FontSize;

        return LoadTextureFromImage(manager->m_Image);
    }
#endif

static unsigned int NumOfPixelsInOneRow(TextureManager* manager)
{
    return manager->m_FontSize * manager->m_TextureWidth;
}

static void WriteOneChar(TextureManager* manager, unsigned char* data, int character, bool wordWrap)
{
    // グリフを読み込み
    int error = FT_Load_Char(manager->m_ftFace, character, FT_LOAD_RENDER);
    if (error != 0)
    {
        printf("FT_Load_Char failed with error: %d\n", error);
        return;
    }

    FT_GlyphSlot slot = manager->m_ftFace->glyph;

    // 文字を表示する横幅の調整
    manager->m_currentX += slot->bitmap_left;

    if (wordWrap)
    {
        // テクスチャ幅をオーバーする場合は先に改行しておく
        if (manager->m_currentX + slot->bitmap.width >= manager->m_TextureWidth)
        {
            manager->m_currentRow++;
            //描画スタート位置を次の行の先頭に
            manager->m_currentX = slot->bitmap_left;
        }
    }

    // この文字を書き込むべきオフセット位置
    unsigned int bufferOffset = 4 * (manager->m_currentX + manager->m_currentRow * NumOfPixelsInOneRow(manager));

    // オフセットを起点として、この文字を書き込み処理を始める高さ
    int glyfY = manager->m_FontSize - slot->bitmap_top;

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
            unsigned int writeIndex = bufferOffset + 4 * (glyfX + glyfY * manager->m_TextureWidth);
            data[writeIndex] = pixColor;
            data[writeIndex + 1] = pixColor;
            data[writeIndex + 2] = pixColor;
            data[writeIndex + 3] = slot->bitmap.buffer[i];
        }
    }

    // 次の文字の書き込み位置の計算
    manager->m_currentX += slot->bitmap.width;
}

static char* GetFontFilepath()
{
#if defined(WIN32)
    static char fontFilepath[FONT_FILEPATH_MAX_LEN];
    static char filepathBuffer[FILEPATH_BUFFER_LEN];
    static wchar_t filepathBufferW[FILEPATH_BUFFER_LEN];
    static bool has_initialized = false;

    if (has_initialized)
        return fontFilepath;

    DWORD size = GetModuleFileNameW(NULL, filepathBufferW, FILEPATH_BUFFER_LEN);
    if (size < 0)
        return NULL;
    for (int i = size - 1; 0 <= i; --i)
    {
        if (filepathBufferW[i] == L'\\')
        {
            filepathBufferW[i] = '\0';
            break;
        }
    }
    // Can't convert paths including Japanese texts...
    if (wcstombs(filepathBuffer, filepathBufferW, FILEPATH_BUFFER_LEN) == -1)
        return NULL;
    snprintf(fontFilepath, FONT_FILEPATH_MAX_LEN, "%s/%s", filepathBuffer, FONT_FILENAME);

    has_initialized = true;
    return fontFilepath;
#elif defined(APPLE)
    static char fontFilepath[FONT_FILEPATH_MAX_LEN];
    static char filepathBuffer[FILEPATH_BUFFER_LEN];
    static bool has_initialized = false;

    if (has_initialized)
        return fontFilepath;

    uint32_t bufSize = FILEPATH_BUFFER_LEN;
    if (_NSGetExecutablePath(filepathBuffer, &bufSize) != 0 || bufSize <= 0)
        return NULL;

    // RaylibIMEInputSampleApp.app/Contents/MacOS/RaylibIMEInputSampleApp
    // RaylibIMEInputSampleApp.app/Contents/Resources/
    int depthBacked = 0;
    for (int i = bufSize - 1; 0 <= i; --i)
    {
        if (filepathBuffer[i] == '/')
        {
            depthBacked++;
            if (depthBacked == 2)
            {
                filepathBuffer[i] = '\0';
                break;
            }
        }
    }
    snprintf(fontFilepath, FONT_FILEPATH_MAX_LEN, "%s/Resources/%s", filepathBuffer, FONT_FILENAME);

    has_initialized = true;
    return fontFilepath;
#elif defined(UNIX)
    static char fontFilepath[FONT_FILEPATH_MAX_LEN];
    static char filepathBuffer[FILEPATH_BUFFER_LEN];
    static bool has_initialized = false;

    if (has_initialized)
        return fontFilepath;

    ssize_t size = readlink("/proc/self/exe", filepathBuffer, FILEPATH_BUFFER_LEN);
    if (size < 0)
        return NULL;
    for (int i = size - 1; 0 <= i; --i)
    {
        if (filepathBuffer[i] == '/')
        {
            filepathBuffer[i] = '\0';
            break;
        }
    }
    snprintf(fontFilepath, FONT_FILEPATH_MAX_LEN, "%s/%s", filepathBuffer, FONT_FILENAME);

    has_initialized = true;
    return fontFilepath;
#endif
    return NULL;
}
