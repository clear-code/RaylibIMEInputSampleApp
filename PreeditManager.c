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

#include "PreeditManager.h"

#include "raylib.h"
#include <string.h>

#define MAX_PREDIT_LEN 128

static int s_PreeditArray[MAX_PREDIT_LEN];
static int s_PreeditLen = 0;

static const int s_PreeditWindowPosXOffset = 15;
static const int s_PreeditWindowPosYOffset = 5;

static PreeditManager_OnPreeditChanged s_OnPreeditChanged;

static void OnPreedit(int preeditLength, int* preeditString, int blockCount,
                      int* blockSizes, int focusedBlock, int caret);

void PreeditManager_Init()
{
    SetPreeditCallback(OnPreedit);
}

void PreeditManager_UpdateWindowPos(int x, int y)
{
    SetPreeditWindowPosition(x + s_PreeditWindowPosXOffset, y + s_PreeditWindowPosYOffset);
}

void PreeditManager_SetOnPreeditChanged(PreeditManager_OnPreeditChanged onPreeditChanged)
{
    s_OnPreeditChanged = onPreeditChanged;
}

void PreeditManager_GetPreeditText(int* text)
{
    memcpy(text, s_PreeditArray, sizeof(int) * s_PreeditLen);
}

static void OnPreedit(int preeditLength, int* preeditString, int blockCount,
                      int* blockSizes, int focusedBlock, int caret)
{
    int blockIndex = -1, remainingBlockSize = 0;

    s_PreeditLen = 0;

    if (preeditLength == 0 || blockCount == 0)
    {
        if (s_OnPreeditChanged != NULL)
            s_OnPreeditChanged(s_PreeditArray, s_PreeditLen);
        return;
    }

    for (int i = 0; i < preeditLength; i++)
    {
        if (i == caret)
            s_PreeditArray[s_PreeditLen++] = '|';

        if (remainingBlockSize == 0)
        {
            if (blockIndex == focusedBlock)
                s_PreeditArray[s_PreeditLen++] = ']';

            blockIndex++;
            remainingBlockSize = blockSizes[blockIndex];

            if (blockIndex == focusedBlock)
                s_PreeditArray[s_PreeditLen++] = '[';
        }

        s_PreeditArray[s_PreeditLen++] = preeditString[i];

        remainingBlockSize--;
    }
    if (blockIndex == focusedBlock)
        s_PreeditArray[s_PreeditLen++] = ']';
    if (caret == preeditLength)
        s_PreeditArray[s_PreeditLen++] = '|';

    if (s_OnPreeditChanged != NULL)
        s_OnPreeditChanged(s_PreeditArray, s_PreeditLen);
}
