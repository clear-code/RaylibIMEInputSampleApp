// Copyright (C) 2022 Daijiro Fukuda <fukuda@clear-code.com>

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

#include "TextEditor.h"

#include "CFreeTypeManager.h"
#include "PreeditManager.h"

#define MAX_TEXT_NUM 1024

static int s_PosX;
static int s_PosY;
static int s_Width;
static int s_Height;

static FreeTypeManager s_FtManager;

static int s_TextCommitted[MAX_TEXT_NUM];
static int s_TextCommittedNum = 0;

static Texture2D s_TextureCommitted;
static Texture2D s_TexturePreedit;

static bool s_NeedToUpdateTexture = false;

static void OnPreeditChanged(unsigned int* preedit, unsigned int length)
{
    s_TexturePreedit = FreeTypeManager_OutputRaylibImage(&s_FtManager, preedit, length, true);
}

bool TextEditor_Init(int posX, int posY, int width, int height, const char* fontFilepath)
{
    s_PosX = posX;
    s_PosY = posY;
    s_Width = width;
    s_Height = height;

    bool has_succeeded = FreeTypeManager_Initialize(&s_FtManager, fontFilepath, s_Width, s_Height, 16);
    if (!has_succeeded) {
        printf("Error: FreeTypeManager failed to initialize.\n");
        return false;
    }

    s_TextureCommitted = FreeTypeManager_OutputRaylibImage(&s_FtManager, U"", 0, false);
    s_TexturePreedit = FreeTypeManager_OutputRaylibImage(&s_FtManager, U"", 0, true);

    PreeditManager_Init();
    PreeditManager_UpdateWindowPos(s_PosX, s_PosY + s_FtManager.m_FontSize);
    PreeditManager_SetOnPreeditChanged(OnPreeditChanged);

    return true;
}

void TextEditor_Update()
{
    int unicodePoint = GetCharPressed();
    while (unicodePoint > 0)
    {
        if (unicodePoint >= 32 && s_TextCommittedNum < MAX_TEXT_NUM)
        {
            s_TextCommitted[s_TextCommittedNum++] = unicodePoint;
            s_NeedToUpdateTexture = true;
        }

        unicodePoint = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && s_TextCommittedNum > 0)
    {
        s_TextCommittedNum--;
        s_NeedToUpdateTexture = true;
    }
}

void TextEditor_Draw()
{
    if (s_NeedToUpdateTexture)
    {
        s_TextureCommitted = FreeTypeManager_OutputRaylibImage(&s_FtManager, s_TextCommitted,
                                                               s_TextCommittedNum, false);
        PreeditManager_UpdateWindowPos(s_PosX + s_FtManager.m_CursorPosX,
                                       s_PosY + s_FtManager.m_CursorPosY + s_FtManager.m_FontSize);
        s_NeedToUpdateTexture = false;
    }

    DrawRectangleLines(s_PosX, s_PosY, s_Width, s_Height, BLACK);
    DrawTexture(s_TextureCommitted, s_PosX, s_PosY, WHITE);
    DrawTexture(s_TexturePreedit, s_PosX + s_FtManager.m_CursorPosX,
                s_PosY + s_FtManager.m_CursorPosY, WHITE);
}
