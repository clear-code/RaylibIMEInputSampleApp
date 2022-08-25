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

#include "config.h"
#include "CFreeTypeManager.h"
#include "PreeditManager.h"

#define MAX_TEXT_NUM 1024

static FreeTypeManager ftManager;
static Texture2D texturePreedit;

static void OnPreeditChanged(unsigned int* preedit, unsigned int length)
{
    texturePreedit = FreeTypeManager_OutputRaylibImage(&ftManager, preedit, length);
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "RaylibIMEInputSampleApp");
    // ToggleFullscreen();

    PreeditManager_Init();
    PreeditManager_UpdateWindowPos(10, 20);
    PreeditManager_SetOnPreeditChanged(OnPreeditChanged);

    bool has_succeeded = FreeTypeManager_Initializ(&ftManager, FONT_FILEPATH, screenWidth, screenHeight, 16);
    if (!has_succeeded) {
        printf("FreeTypeManager_Initializ failed. Can't start the app.\n");
        return 1;
    }

    SetTargetFPS(30);               // Set our game to run at 60 frames-per-second

    unsigned int unicode_points[MAX_TEXT_NUM];
    unsigned int unicode_points_num = 0;

    Texture2D texture = FreeTypeManager_OutputRaylibImage(&ftManager, U"", 0);
    texturePreedit = FreeTypeManager_OutputRaylibImage(&ftManager, U"", 0);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        bool has_text_updated = false;

        int key = GetCharPressed();
        while (key > 0)
        {
            if (key >= 32 && unicode_points_num < MAX_TEXT_NUM)
            {
                unicode_points[unicode_points_num++] = key;
                has_text_updated = true;
            }

            key = GetCharPressed();
        }

        if (has_text_updated)
            texture = FreeTypeManager_OutputRaylibImage(&ftManager, unicode_points, unicode_points_num);

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(texturePreedit, 0, 0, WHITE);
        DrawTexture(texture, 0, 20, WHITE);

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
