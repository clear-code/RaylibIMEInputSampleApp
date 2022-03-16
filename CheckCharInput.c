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

#include "CheckCharInput.h"
#include "raylib.h"

#include <string.h>

#define     INPUT_TEXTARRAY_MAX     64

static wchar_t s_InputTextArray[INPUT_TEXTARRAY_MAX];
static unsigned int s_InputTextNum = 0;

//入力があるかどうか、入力があった場合入力された文字の長さを返す
unsigned int GetInputCharNum()
{
    wchar_t buf[64];
    buf[0] = GetCharPressed();
    int input_num = 0;
    while(buf[input_num] != 0)
    {
        input_num += 1;
        buf[input_num] = GetCharPressed();
    }
    buf[input_num + 1] = U'\0';

    s_InputTextNum = input_num;
    memcpy(s_InputTextArray, buf, sizeof(wchar_t) * (s_InputTextNum + 1));
    
    return s_InputTextNum;
}

//入力された文字を取得する
void GetInputChar(wchar_t* input_text)
{
    memcpy(input_text, s_InputTextArray, sizeof(wchar_t) * (s_InputTextNum + 1));
}
