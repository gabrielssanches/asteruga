#include "asteruga.h"

static struct ship tf_s1;
//static struct space tf_space;

static void testfield_enter(game_context_t *gctx) {
    //Rectangle space_limits = { 0, 0, GetScreenWidth(), GetScreenHeight() };
    //space_init(&tf_space, space_limits);

    Vector2 pos_init = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    _ship_init(&tf_s1, &pos_init);
    _ship_spawn(&tf_s1);
}

static void testfield_update(game_context_t *gctx) {
    _ship_update(&tf_s1);
}

static void testfield_draw(game_context_t *gctx) {
    BeginDrawing();

    ClearBackground(COLOR_BACKGROUND);

    Camera2D cam = { 0 };
    cam.target = Vector2Zero();
    cam.offset = Vector2Zero();
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;

    BeginMode2D(cam);

    _ship_draw(&tf_s1);

    //bullet_draw(&s1.bullets);

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
