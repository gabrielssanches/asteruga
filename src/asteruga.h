#ifndef ASTERUGA_H_ 
#define ASTERUGA_H_ 

#include <raylib.h>
#include <raygui.h>
#include <raymath.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#define DEBUG_PRINT(fmt, ...) \
    do { \
        fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

#define COLOR_BACKGROUND    RAYWHITE
#define COLOR_SHIP          BLACK
#define COLOR_NEUTRAL       BLACK 
#define COLOR_A             RED
#define COLOR_B             BLUE

typedef struct game_context game_context_t;

enum state_name {
    STATE_LOAD,
    STATE_TESTFIELD,
    STATE_INTRO_RAYLIB,
    STATE_MAIN_MENU,
    STATE_GAME,
    NUMBER_OF_STATES,
};

enum soundfx {
    SFX_MENU_SELECT,
    SFX_MENU_MOVE,
    SFX_SHIP_SHOOT,
    SFX_SHIP_POWERUP,
    SFX_SHIP_EXPLOSION,
    SFX_ASTEROID_HIT,
    SFX_ASTEROID_HIT_PLUS,
    SFX_ASTEROID_EXPLOSION,
    SFX_BOSS_DEFEAT,
    SFX_MAX,
};

enum theme {
    THEME_LEVEL2,
    THEME_BOSS,
    THEME_MAX,
};

typedef void (*init_func_t)(game_context_t *gctx);

struct game_state {
    init_func_t init;
    init_func_t deinit;
    init_func_t enter;
    init_func_t exit;
    init_func_t update;
    init_func_t draw;
};

struct game_context {
    bool audio_init;
    bool quit;
    struct game_state *state_current;
    struct game_state *state_next;
    struct game_state *states[NUMBER_OF_STATES];
    int level_sel;
    Sound sfx[SFX_MAX];
    Music theme[THEME_MAX];
    Music *theme_playing;
};

void game_state_change(struct game_context *gctx, enum state_name next_state);
void game_sfx_play(struct game_context *gctx, enum soundfx sfx_n);
void game_theme_play(struct game_context *gctx, enum theme theme);
void game_theme_stop(struct game_context *gctx);
void game_theme_update(struct game_context *gctx);
void game_theme_restart(struct game_context *gctx);
void game_score_push(int level, int score);

extern struct game_state _gs_load;
extern struct game_state _gs_intro_raylib;
extern struct game_state _gs_main_menu;
extern struct game_state _gs_game;
extern struct game_state gamestate_testfield; 

#include "ga_all.h"
#include "ga_ship.h"
#include "ga_space.h"

// raylib extension functions

void DrawTextCentered(const char *text, int pos_x, int pos_y, int fontSize, Color color);

#endif // ASTERUGA_H_ 
