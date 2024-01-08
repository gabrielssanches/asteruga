#include "asteruga.h"

static int load_progress;

static void load_enter(struct game_context *gctx) {
    load_progress = 100;
}

static void load_update(struct game_context *gctx) {
    if (GetKeyPressed() != 0 ||
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (load_progress >= 100) {
            game_state_change(gctx, STATE_INTRO_RAYLIB);
        }
    }
}

static void load_draw(struct game_context *gctx) {
    BeginDrawing();

    ClearBackground(COLOR_BACKGROUND);

    if (load_progress == 100) {
        DrawTextCentered("Press any key/Click/Touch to start", GetScreenWidth()/2, GetScreenHeight()/2, 30, COLOR_NEUTRAL);
    } else {
        DrawTextCentered(TextFormat("Loading... %i", load_progress/2), GetScreenWidth()/2, GetScreenHeight()/2, 40, COLOR_NEUTRAL);
    }

    EndDrawing();
}

struct game_state _gs_load = {
    .init = NULL,
    .deinit = NULL,
    .enter = load_enter,
    .exit = NULL,
    .update = load_update,
    .draw = load_draw
};
