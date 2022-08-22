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

#ifndef CheckPreedit_h
#define CheckPreedit_h

#include <stdio.h>
#include <wchar.h>

typedef void (*PreeditManager_OnPreeditChanged)(wchar_t* preedit);

void PreeditManager_Init();

void PreeditManager_UpdateWindowPos(int x, int y);

void PreeditManager_SetOnPreeditChanged(PreeditManager_OnPreeditChanged onPreeditChanged);

void PreeditManager_GetPreeditChar(wchar_t* input_text);

#endif /* CheckPreedit_h */
