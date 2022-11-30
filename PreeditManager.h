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

#ifndef PreeditManager_h
#define PreeditManager_h

#include <stdio.h>

#include "config.h"

typedef void (*PreeditManager_OnPreeditChanged)(int* preedit, int length);

void PreeditManager_Init();

void PreeditManager_UpdateCursorRectangle(int x, int y, int w, int h);

void PreeditManager_SetOnPreeditChanged(PreeditManager_OnPreeditChanged onPreeditChanged);

void PreeditManager_GetPreeditText(int* text);

#ifdef MANAGE_PREEDIT_CANDIDATE
    typedef void (*PreeditManager_OnPreeditCandidateChanged)(int** candidates, int* lengths, int pageSize, int selectedIndex);
    void PreeditManager_SetOnPreeditCandidateChanged(PreeditManager_OnPreeditCandidateChanged onPreeditCandidateChanged);
#endif

#endif /* PreeditManager_h */
