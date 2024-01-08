#include "asteruga.h"

static int framesCounter = 0;
static int finishScreen = 0;
static int logoPositionX = 0;
static int logoPositionY = 0;
static int lettersCount = 0;
static int topSideRecWidth = 0;
static int leftSideRecHeight = 0;
static int bottomSideRecWidth = 0;
static int rightSideRecHeight = 0;
static int state = 0;              // Logo animation states
static float alpha = 1.0f;         // Useful for fading

static void intro_raylib_enter(struct game_context *gctx) {
    if (!gctx->audio_init) {
        InitAudioDevice(); // init audio here since user is in context (mainly for the web)
        gctx->audio_init = true;
    }
    gctx->sfx[SFX_MENU_SELECT] = LoadSound("src/resources/menu_select.wav");;
    gctx->sfx[SFX_MENU_MOVE] = LoadSound("src/resources/menu_move.wav");;

    gctx->sfx[SFX_SHIP_POWERUP] = LoadSound("src/resources/powerup.wav");;
    gctx->sfx[SFX_SHIP_SHOOT] = LoadSound("src/resources/ship_shoot.wav");;
    gctx->sfx[SFX_SHIP_EXPLOSION] = LoadSound("src/resources/ship_explosion.wav");;

    gctx->sfx[SFX_ASTEROID_EXPLOSION] = LoadSound("src/resources/asteroid_explosion.wav");;
    gctx->sfx[SFX_ASTEROID_HIT] = LoadSound("src/resources/asteroid_hit.wav");;
    gctx->sfx[SFX_ASTEROID_HIT_PLUS] = LoadSound("src/resources/asteroid_hit_plus.wav");;
    gctx->sfx[SFX_BOSS_DEFEAT] = LoadSound("src/resources/boss_defeat.wav");;

    gctx->theme[THEME_LEVEL2] = LoadMusicStream("src/resources/level2_theme.wav");
    gctx->theme[THEME_LEVEL2].looping = true;
    gctx->theme[THEME_BOSS] = LoadMusicStream("src/resources/boss_theme.wav");
    gctx->theme[THEME_BOSS].looping = true;
    gctx->theme_playing = NULL;

    finishScreen = 0;
    framesCounter = 0;
    lettersCount = 0;

    logoPositionX = GetScreenWidth()/2 - 128;
    logoPositionY = GetScreenHeight()/2 - 128;

    topSideRecWidth = 16;
    leftSideRecHeight = 16;
    bottomSideRecWidth = 16;
    rightSideRecHeight = 16;

    state = 0;
    alpha = 1.0f;
}

static void intro_raylib_update(struct game_context *gctx) {
    // skip intro
    if (GetKeyPressed() != 0 ||
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game_state_change(gctx, STATE_MAIN_MENU);
    }
    if (state == 0)                 // State 0: Top-left square corner blink logic
    {
        framesCounter++;

        if (framesCounter == 80)
        {
            state = 1;
            framesCounter = 0;      // Reset counter... will be used later...
        }
    }
    else if (state == 1)            // State 1: Bars animation logic: top and left
    {
        topSideRecWidth += 8;
        leftSideRecHeight += 8;

        if (topSideRecWidth == 256) state = 2;
    }
    else if (state == 2)            // State 2: Bars animation logic: bottom and right
    {
        bottomSideRecWidth += 8;
        rightSideRecHeight += 8;

        if (bottomSideRecWidth == 256) state = 3;
    }
    else if (state == 3)            // State 3: "raylib" text-write animation logic
    {
        framesCounter++;

        if (lettersCount < 10)
        {
            if (framesCounter/12)   // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }
        }
        else    // When all letters have appeared, just fade out everything
        {
            if (framesCounter > 200)
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    finishScreen = 1;   // Jump to next screen
                    game_state_change(gctx, STATE_MAIN_MENU);
                }
            }
        }
    }
}

static void intro_raylib_draw(struct game_context *gctx) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (state == 0)         // Draw blinking top-left square corner
    {
        if ((framesCounter/10)%2) DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);
    }
    else if (state == 1)    // Draw bars animation: top and left
    {
        DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
        DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
    }
    else if (state == 2)    // Draw bars animation: bottom and right
    {
        DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
        DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);

        DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);
        DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);
    }
    else if (state == 3)    // Draw "raylib" text-write animation + "powered by"
    {
        DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, Fade(BLACK, alpha));
        DrawRectangle(logoPositionX, logoPositionY + 16, 16, leftSideRecHeight - 32, Fade(BLACK, alpha));

        DrawRectangle(logoPositionX + 240, logoPositionY + 16, 16, rightSideRecHeight - 32, Fade(BLACK, alpha));
        DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, Fade(BLACK, alpha));

        DrawRectangle(GetScreenWidth()/2 - 112, GetScreenHeight()/2 - 112, 224, 224, Fade(RAYWHITE, alpha));

        DrawText(TextSubtext("raylib", 0, lettersCount), GetScreenWidth()/2 - 44, GetScreenHeight()/2 + 48, 50, Fade(BLACK, alpha));

        if (framesCounter > 20) DrawText("powered by", logoPositionX, logoPositionY - 27, 20, Fade(DARKGRAY, alpha));
    }

    EndDrawing();
}

struct game_state _gs_intro_raylib = {
    .init = NULL,
    .deinit = NULL,
    .enter = intro_raylib_enter,
    .exit = NULL,
    .update = intro_raylib_update,
    .draw = intro_raylib_draw
};
