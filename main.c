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
#include "TextEditor.h"

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

    DrawText(TextFormat("IME candidate window pos (x, y): (%d, %d)", candidateWindowPosX, candidateWindowPosY),
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
    const int editorMargin = 10;

    InitWindow(screenWidth, screenHeight, "RaylibIMEInputSampleApp");
    // ToggleFullscreen();

    TextEditor_Init(editorMargin, editorStartY + editorMargin,
                    screenWidth - 2 * editorMargin,
                    screenHeight - editorStartY - 2 * editorMargin,
                    FONT_FILEPATH);

    SetTargetFPS(30); 
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        TextEditor_Update();
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawPreeditUtils();
        TextEditor_Draw();

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
