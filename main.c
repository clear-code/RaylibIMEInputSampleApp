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

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "config.h"
#include "CFreeTypeManager.h"
#include "PreeditManager.h"

#define MAX_TEXT_NUM 1024

static FreeTypeManager ftManager;
static Texture2D texturePreedit;

static void OnPreeditChanged(unsigned int* preedit, unsigned int length)
{
    texturePreedit = FreeTypeManager_OutputRaylibImage(&ftManager, preedit, length, true);
}

static void DrawPreeditUtils()
{
    int candidateWindowPosX;
    int candidateWindowPosY;
    GetPreeditWindowPosition(&candidateWindowPosX, &candidateWindowPosY);

    bool isImeOn = IsImeOn();

    int x = 10;
    int y = 0;

    if (isImeOn)
        DrawText("IME status: ON", x, y, 20, BLACK);
    else
        DrawText("IME status: OFF", x, y, 20, BLACK);
    y += 22;

    DrawText(TextFormat("IME candidate window pos (x, y): (%d, %d)\n", candidateWindowPosX, candidateWindowPosY),
             x, y, 20, BLACK);
    y += 22;

    y += 5;
    if (GuiButton((Rectangle){ x, y, 200, 30 }, "Toggle IME Status"))
        SetImeStatus(!isImeOn);

    x += 210;
    if (GuiButton((Rectangle){ x, y, 200, 30 }, "Reset Preedit"))
        ResetPreedit();
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    const int screenWidth = 800;
    const int screenHeight = 450;
    const int editorStartY = 100;
    const int preeditWindowPosXOffset = 15;
    const int preeditWindowPosYOffset = 5;

    InitWindow(screenWidth, screenHeight, "RaylibIMEInputSampleApp");
    // ToggleFullscreen();

    bool has_succeeded = FreeTypeManager_Initialize(&ftManager, FONT_FILEPATH, screenWidth, screenHeight, 16);
    if (!has_succeeded) {
        printf("FreeTypeManager_Initializ failed. Can't start the app.\n");
        return 1;
    }

    PreeditManager_Init();
    PreeditManager_UpdateWindowPos(preeditWindowPosXOffset,
                                   editorStartY + ftManager.m_FontSize + preeditWindowPosYOffset);
    PreeditManager_SetOnPreeditChanged(OnPreeditChanged);

    SetTargetFPS(30);               // Set our game to run at 60 frames-per-second

    unsigned int unicode_points[MAX_TEXT_NUM];
    unsigned int unicode_points_num = 0;

    Texture2D texture = FreeTypeManager_OutputRaylibImage(&ftManager, U"", 0, false);
    texturePreedit = FreeTypeManager_OutputRaylibImage(&ftManager, U"", 0, true);
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

        if (IsKeyPressed(KEY_BACKSPACE) && unicode_points_num > 0)
        {
            unicode_points_num--;
            has_text_updated = true;
        }

        if (has_text_updated)
        {
            texture = FreeTypeManager_OutputRaylibImage(&ftManager, unicode_points, unicode_points_num, false);
            PreeditManager_UpdateWindowPos(ftManager.m_CursorPosX + preeditWindowPosXOffset,
                                           editorStartY + ftManager.m_CursorPosY + ftManager.m_FontSize + preeditWindowPosYOffset);
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawPreeditUtils();
        DrawTexture(texture, 0, editorStartY, WHITE);
        DrawTexture(texturePreedit, ftManager.m_CursorPosX, editorStartY + ftManager.m_CursorPosY, WHITE);

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
