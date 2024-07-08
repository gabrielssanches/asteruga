#include "asteruga.h"

#include <threads.h>

#define PADDLE_WIDTH            10
#define PADDLE_HEIGHT           100
#define BALL_SIZE               10

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static struct game_context gctx = {0};

void game_state_change(struct game_context *gctx, enum state_name next_state) {
    if (next_state < NUMBER_OF_STATES) {
        gctx->state_next = gctx->states[next_state];
    }
}

void game_sfx_play(struct game_context *gctx, enum soundfx sfx_n) {
    PlaySound(gctx->sfx[sfx_n]);
}

void game_theme_restart(struct game_context *gctx) {
    if (gctx->theme_playing != NULL) {
        StopMusicStream(*gctx->theme_playing);
        PlayMusicStream(*gctx->theme_playing);
    }
}

void game_theme_play(struct game_context *gctx, enum theme theme) {
    if (gctx->theme_playing != NULL) {
        StopMusicStream(*gctx->theme_playing);
    }
    gctx->theme_playing = &gctx->theme[theme];
    PlayMusicStream(*gctx->theme_playing);
}

void game_theme_update(struct game_context *gctx) {
    if (gctx->theme_playing != NULL) {
        UpdateMusicStream(*gctx->theme_playing);
    }
}

void game_theme_stop(struct game_context *gctx) {
    if (gctx->theme_playing != NULL) {
        StopMusicStream(*gctx->theme_playing);
    }
    gctx->theme_playing = NULL;
}

struct game_state _gs_none = {
    .init = NULL,
    .deinit = NULL,
    .enter = NULL,
    .exit = NULL,
    .update = NULL,
    .draw = NULL
};

static void UpdateDrawFrame(void) {
    if (gctx.state_current != gctx.state_next) {
        if (gctx.state_current->exit != NULL) {
            gctx.state_current->exit(&gctx);
        }
        if (gctx.state_next->enter != NULL) {
            gctx.state_next->enter(&gctx);
        }
        gctx.state_current = gctx.state_next;
    }
    gctx.state_current->update(&gctx);
    gctx.state_current->draw(&gctx);
}

int main(int argc, char *argv[]) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(1920, 1080, "asteruga");

    DisableCursor();

    gctx.state_current = &_gs_none;
    gctx.state_next = &_gs_load;
    gctx.states[STATE_LOAD] = &_gs_load;
    gctx.states[STATE_TESTFIELD] = &gamestate_testfield;
    gctx.states[STATE_INTRO_RAYLIB] = &_gs_intro_raylib;
    gctx.states[STATE_MAIN_MENU] = &_gs_main_menu;
    gctx.states[STATE_GAME] = &_gs_game;

    // init all states
    for (int i = 0; i < NUMBER_OF_STATES; i++) {
        if (gctx.states[i]->init != NULL) {
            gctx.states[i]->init(&gctx);
        }
    }

#if defined(PLATFORM_WEB)
    EM_ASM({ window.onkeydown = function(e) { return !(e.keyCode == 32); }; });
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    while (!gctx.quit && !WindowShouldClose()) {   // Detect window close button or ESC key
        UpdateDrawFrame();
    }
#endif

    // deinit all states
    for (int i = 0; i < NUMBER_OF_STATES; i++) {
        if (gctx.states[i]->deinit != NULL) {
            gctx.states[i]->deinit(&gctx);
        }
    }

   if (gctx.audio_init) {
       CloseAudioDevice();
   }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

