#include "asteruga.h"

static void testfield_enter(game_context_t *gctx) {

}

static void testfield_exit(game_context_t *gctx) {

}

static void testfield_update(game_context_t *gctx) {

}

static void testfield_draw(game_context_t *gctx) {

}

struct game_state gamestate_testfield = {
    .init = NULL,
    .deinit = NULL,
    .enter = testfield_enter,
    .exit = testfield_exit,
    .update = testfield_update,
    .draw = testfield_draw
};
