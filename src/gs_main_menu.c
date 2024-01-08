#include "asteruga.h"

#define LEVELS_N 4

static int level_sel;

struct level_meta {
    int stars;
};

static struct level_meta level_metas[4] = {
    { .stars = 0 },
    { .stars = 0 },
    { .stars = 0 },
    { .stars = 0 },
};

void game_score_push(int level, int score) {
    if (level_metas[level].stars < score) {
        level_metas[level].stars = score;
    }
}

static void main_menu_enter(struct game_context *gctx) {
    level_sel = 0;
}

static void main_menu_update(struct game_context *gctx) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        gctx->quit = true;
    }
    if (IsKeyPressed(KEY_ENTER) ||
        IsKeyPressed(KEY_SPACE)) {

        gctx->level_sel = level_sel;
        game_sfx_play(gctx, SFX_MENU_SELECT);
        game_state_change(gctx, STATE_GAME);
    }
    if (IsKeyPressed(KEY_D) ||
        IsKeyPressed(KEY_RIGHT)) {
        level_sel++;
        game_sfx_play(gctx, SFX_MENU_MOVE);
    }
    if (IsKeyPressed(KEY_A) ||
        IsKeyPressed(KEY_LEFT)) {
        level_sel--;
        game_sfx_play(gctx, SFX_MENU_MOVE);
    }
    if (level_sel >= LEVELS_N) {
        level_sel = 0;
    }
    if (level_sel < 0) {
        level_sel = LEVELS_N -1;
    }
}

#define LEVEL_BOX_W 150
#define LEVEL_BOX_SPACE_W 30
#define LEVEL_BOX_H 100

static void level_sel_draw(const char *name, int sel, int selected) {
    Vector2 sel_pos = { GetScreenWidth()/2 +LEVEL_BOX_SPACE_W/2 -(LEVEL_BOX_W+LEVEL_BOX_SPACE_W)*2 +LEVEL_BOX_W/2 +(sel*(LEVEL_BOX_W +LEVEL_BOX_SPACE_W)), GetScreenHeight()/2 +LEVEL_BOX_H };
    Rectangle rect_sel = { sel_pos.x -LEVEL_BOX_W/2, sel_pos.y -LEVEL_BOX_H/2, LEVEL_BOX_W, LEVEL_BOX_H };

    Color color = COLOR_NEUTRAL;
    if (selected == sel) {
        color = COLOR_A;
    }
    DrawRectangleLinesEx(rect_sel, 5.0f, color);
    DrawTextCentered(name, sel_pos.x, sel_pos.y, 40, COLOR_NEUTRAL);

    sel_pos.x += (10 -LEVEL_BOX_W/2);
    sel_pos.y += (15 +LEVEL_BOX_H/2);
    for (int i = 0; i < 3; i++) {
        if (i < level_metas[sel].stars) {
            DrawPoly(sel_pos, 3, 10.0f, -90.0f, COLOR_B);
        }
        DrawPolyLinesEx(sel_pos, 3, 10.0f, -90.0f, 3.0f,COLOR_NEUTRAL);
        sel_pos.x += 20.0f;
    }
}

static void main_menu_draw(struct game_context *gctx) {
    BeginDrawing();

    ClearBackground(COLOR_BACKGROUND);

    DrawTextCentered("Asteruga", GetScreenWidth()/2, GetScreenHeight()/2 -200, 100, COLOR_NEUTRAL);

    DrawTextCentered("choose level", GetScreenWidth()/2, GetScreenHeight()/2, 25, COLOR_NEUTRAL);

    level_sel_draw("level 1", 0, level_sel);
    level_sel_draw("level 2", 1, level_sel);
    level_sel_draw("level 3", 2, level_sel);
    level_sel_draw("BOSS", 3, level_sel);

    DrawText("made for Raylib Slo-Jam 2023/2024", 100, GetScreenHeight() -100, 20, COLOR_NEUTRAL);
    DrawText("source at github.com/gabrielssanches/asteruga", 100, GetScreenHeight() -140, 20, COLOR_NEUTRAL);

    EndDrawing();
}

struct game_state _gs_main_menu = {
    .init = NULL,
    .deinit = NULL,
    .enter = main_menu_enter,
    .exit = NULL,
    .update = main_menu_update,
    .draw = main_menu_draw
};
