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
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "config.h"
#include "TextEditor.h"

static bool s_InitAsFullscreen = false;

static int s_ScreenWidth = 800;
static int s_ScreenHeight = 450;
// The sizes of the screen as windowed (not fullscreened).
// Used for toggling windowed/fullscreened.
static int s_WindowedWidth = 800;
static int s_WindowedHeight = 450;

// Call "ToggleFullscreen" after this frame count.
// On X11, updating the window size is not applied in the same frame,
// so need this to toggle to the fullscreen with the specified size.
static int s_DelayFrameCountToFullscreen = 0;

static bool ParseArgs(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            printf("Fullscreen.\n");
            s_InitAsFullscreen = true;
            SetConfigFlags(FLAG_FULLSCREEN_MODE);
            continue;
        }

        if (strcmp(argv[i], "-r") == 0)
        {
            printf("Resizable.\n");
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            continue;
        }

        printf("Options:\n");
        printf("  -f use full screen\n");
        printf("  -r make window resizable\n");
        return false;
    }

    return true;
}

static bool IsTogglingFullscreen()
{
    if (s_DelayFrameCountToFullscreen > 0)
        return true;
    return false;
}

static void DrawPreeditUtils()
{
    int cursorX;
    int cursorY;
    int cursorWidth;
    int cursorHeight;
    GetPreeditCursorRectangle(&cursorX, &cursorY, &cursorWidth, &cursorHeight);

    bool isImeOn = IsImeOn();

    int x = 10;
    int y = 5;

    DrawText(TextFormat("Screen Size (w, h): (%d, %d)", s_ScreenWidth, s_ScreenHeight),
             x, y, 20, BLACK);
    y += 22;

    if (isImeOn)
        DrawText("IME status: ON", x, y, 20, BLACK);
    else
        DrawText("IME status: OFF", x, y, 20, BLACK);
    y += 22;

    DrawText(TextFormat("preedit cursor (x, y, w, h): (%d, %d, %d, %d)", cursorX, cursorY, cursorWidth, cursorHeight),
             x, y, 20, BLACK);
    y += 22;

    y += 5;
    if (GuiButton((Rectangle){ x, y, 200, 30 }, "Toggle IME Status"))
        SetImeStatus(!isImeOn);

    x += 210;
    if (GuiButton((Rectangle){ x, y, 200, 30 }, "Reset Preedit"))
        ResetPreedit();

    x += 210;
    if (GuiButton((Rectangle){ x, y, 200, 30 }, "Toggle Fullscreen"))
    {
        if (!IsTogglingFullscreen())
        {
            if (IsWindowFullscreen())
            {
                ToggleFullscreen();
                SetWindowSize(s_WindowedWidth, s_WindowedHeight);
            }
            else
            {
                const int monitorIndex = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitorIndex), GetMonitorHeight(monitorIndex));
                // On X11, updating the window size is not applied in the same frame,
                // so need to call "ToggleFullscreen" after some frames.
                s_DelayFrameCountToFullscreen = 3;
            }
        }
    }
}

int main(int argc, char** argv)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!ParseArgs(argc, argv))
        return -1;

    const int editorStartY = 125;
    const int editorMargin = 10;

#ifdef USE_SOFT_FULLSCREEN
    SetConfigFlags(FLAG_SOFT_FULLSCREEN);
#endif
#ifdef MANAGE_PREEDIT_CANDIDATE
    SetConfigFlags(FLAG_MANAGE_PREEDIT_CANDIDATE);
#endif

    if (!s_InitAsFullscreen)
        InitWindow(s_ScreenWidth, s_ScreenHeight, "RaylibIMEInputSampleApp");
    else
    {
        InitWindow(0, 0, "RaylibIMEInputSampleApp");
        s_ScreenWidth = GetScreenWidth();
        s_ScreenHeight = GetScreenHeight();
    }

    TextEditor_Init(editorMargin, editorStartY + editorMargin,
                    s_ScreenWidth - 2 * editorMargin,
                    s_ScreenHeight - editorStartY - 2 * editorMargin,
                    FONT_FILEPATH);

    SetTargetFPS(30); 
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (s_DelayFrameCountToFullscreen > 0 && --s_DelayFrameCountToFullscreen == 0)
            ToggleFullscreen();

        s_ScreenWidth = GetScreenWidth();
        s_ScreenHeight = GetScreenHeight();

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
