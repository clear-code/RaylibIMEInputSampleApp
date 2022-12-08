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

#include "TextureManager.h"
#include "PreeditManager.h"

#define MAX_TEXT_NUM 1024

static int s_FontSize = 16;

static int s_PosX;
static int s_PosY;
static int s_Width;
static int s_Height;

static TextureManager s_TextureManagerForCommitted;

static int s_TextCommitted[MAX_TEXT_NUM];
static int s_TextCommittedNum = 0;

static Texture2D s_TextureCommitted;

static TextureManager s_TextureManagerForPreedit;
static Texture2D s_TexturePreedit;

static bool s_NeedToUpdateTexture = false;

#ifdef MANAGE_PREEDIT_CANDIDATE
    static TextureManager s_TextureManagerForCandidate;
    static Texture2D s_TexturePreeditCandidate;
    static void OnPreeditCandidateChanged(int** candidates, int* lengths, int pageSize, int selectedIndex)
    {
        s_TexturePreeditCandidate = TextureManager_CreateTextureForCandidate(&s_TextureManagerForCandidate, candidates, lengths, pageSize, selectedIndex);
    }
#endif

static void OnPreeditChanged(int* preedit, int length)
{
    s_TexturePreedit = TextureManager_CreateTexture(&s_TextureManagerForPreedit, preedit, length);
}

static int CursorHight()
{
    // It might be possible to get this padding from FreeType, but I don't know how.
    const int padding = 2;
    return s_FontSize + padding;
}

bool TextEditor_Init(int posX, int posY, int width, int height, const char* fontFilepath)
{
    s_PosX = posX;
    s_PosY = posY;
    s_Width = width;
    s_Height = height;

    bool hasSucceeded = TextureManager_Initialize(&s_TextureManagerForCommitted, fontFilepath, s_Width, s_Height, s_FontSize);
    if (!hasSucceeded)
    {
        printf("Error: TextureManager failed to initialize.\n");
        return false;
    }
    hasSucceeded = TextureManager_Initialize(&s_TextureManagerForPreedit, fontFilepath, 400, 200, s_FontSize);
    if (!hasSucceeded)
    {
        printf("Error: TextureManager failed to initialize.\n");
        return false;
    }

    PreeditManager_Init();
    PreeditManager_UpdateCursorRectangle(s_PosX, s_PosY, 1, CursorHight());
    PreeditManager_SetOnPreeditChanged(OnPreeditChanged);

#ifdef MANAGE_PREEDIT_CANDIDATE
    hasSucceeded = TextureManager_Initialize(&s_TextureManagerForCandidate, fontFilepath, 400, 400, s_FontSize);
    if (!hasSucceeded)
    {
        printf("Error: TextureManager failed to initialize.\n");
        return false;
    }

    PreeditManager_SetOnPreeditCandidateChanged(OnPreeditCandidateChanged);
#endif

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
        s_TextureCommitted = TextureManager_CreateTexture(&s_TextureManagerForCommitted, s_TextCommitted, s_TextCommittedNum);
        PreeditManager_UpdateCursorRectangle(s_PosX + s_TextureManagerForCommitted.m_CursorPosX,
                                             s_PosY + s_TextureManagerForCommitted.m_CursorPosY,
                                             1,
                                             CursorHight());
        s_NeedToUpdateTexture = false;
    }

    DrawRectangleLines(s_PosX, s_PosY, s_Width, s_Height, BLACK);
    DrawTexture(s_TextureCommitted, s_PosX, s_PosY, WHITE);
    DrawTexture(s_TexturePreedit,
                s_PosX + s_TextureManagerForCommitted.m_CursorPosX,
                s_PosY + s_TextureManagerForCommitted.m_CursorPosY,
                WHITE);

#ifdef MANAGE_PREEDIT_CANDIDATE
    DrawTexture(s_TexturePreeditCandidate,
                s_PosX + s_TextureManagerForCommitted.m_CursorPosX,
                s_PosY + s_TextureManagerForCommitted.m_CursorPosY + s_FontSize + 10,
                WHITE);
#endif
}
