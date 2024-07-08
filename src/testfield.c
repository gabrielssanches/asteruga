#include "asteruga.h"

static struct ship s1;
static struct space space; 

static void testfield_enter(game_context_t *gctx) {
    space_init(&space, space_rect);

    ship_init(&s1, &space);
}

static void testfield_update(game_context_t *gctx) {
    if (on_key_press(KEY_R)) {
        ship_reset(&s1);
    }

    ship_input_update(&s1);
}

struct bullet {


static void testfield_draw(game_context_t *gctx) {
    BeginDrawing();

    ClearBackground(COLOR_BACKGROUND);

    Camera2D cam = { 0 };
    cam.target = Vector2Zero();
    cam.offset = Vector2Zero();
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;

    BeginMode2D(cam);

    ship_draw(&s1);

    bullet_draw(&s1.bullets);

    EndMode2D();

    Rectangle screen_border = { 0, 0, GetScreenWidth(), GetScreenHeight() };
    DrawRectangleLinesEx(screen_border, 5.0f, RED);

    EndDrawing();
}

struct game_state gamestate_testfield = {
    .init = NULL,
    .deinit = NULL,
    .enter = testfield_enter,
    .exit = NULL,
    .update = testfield_update,
    .draw = testfield_draw
};
