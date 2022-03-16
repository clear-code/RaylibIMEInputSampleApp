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

#include "raylib.h"

#include "CFreeTypeManager.h"
#include "CheckCharInput.h"

int main(void)
{
    
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [text] example - font loading");
    
    char text[] = "文字表示テスト";

    FreeTypeManager ftManager;
    FreeTypeManager_Initializ(&ftManager, "./resources/GenShinGothic-Regular.ttf", screenWidth, screenHeight, 16);
    
    SetTargetFPS(30);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    wchar_t wText[1024];
    unsigned int wText_len = 0;
    
    FreeTypeManager_ConvertUtf8toUtf32(wText, text);
    Texture2D texture = FreeTypeManager_OutputRaylibImage(&ftManager, wText);
    wText_len = wcslen(wText);

    bool change_text = false;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        
        //文字入力確認
        unsigned int input_num = GetInputCharNum();
        if(input_num > 0)
        {
            wchar_t text[64];
            GetInputChar(text);
            
            wcscat(wText, text);
            
            change_text = true;
        }
        
        //入力された文字から画像を作成
        if(change_text == true)
        {
            texture = FreeTypeManager_OutputRaylibImage(&ftManager, wText);
            change_text = false;
        }
        
        //----------------------------------------------------------------------------------
        

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
        
        DrawTexture(texture, 0,0, WHITE);

        DrawFPS(700, 10);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
