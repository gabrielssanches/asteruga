#include "asteruga.h"

#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24

static struct ship tf_s1;
//static struct space tf_space;
static Rectangle settingsRect;
static Rectangle settingsBarRect;
static bool moveBar;

static void testfield_enter(game_context_t *gctx) {
    //Rectangle space_limits = { 0, 0, GetScreenWidth(), GetScreenHeight() };
    //space_init(&tf_space, space_limits);

    Vector2 pos_init = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    _ship_init(&tf_s1, &pos_init);
    _ship_spawn(&tf_s1);

    settingsRect = (Rectangle){ 100, 0, 200, 200 };
    settingsBarRect = settingsRect;
    settingsBarRect.height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    moveBar = false;
}

static void testfield_update(game_context_t *gctx) {
    _ship_update(&tf_s1);

    if (CheckCollisionPointRec(GetMousePosition(), settingsBarRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        printf("Status bar clicked\n");
        moveBar = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        printf("Status bar false\n");
        moveBar = false;
    }
    if (moveBar) {
        settingsRect.x = GetMouseX();
        settingsRect.y = GetMouseY();
        settingsBarRect.x = settingsRect.x;
        settingsBarRect.y = settingsRect.y;
    }
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

    { // GUI
        float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
        const float margin = 8;

        Rectangle contentRect = (Rectangle){ 0, 0, 0, 0 };
        Vector2 scrollOffset = (Vector2){ 0, 0 };
        GuiScrollPanel(settingsRect, "Settings", contentRect, &scrollOffset, NULL);

        contentRect = (Rectangle){ settingsRect.x + margin, settingsRect.y +RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT+margin, settingsRect.width - 2*margin - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH), 0 };

        contentRect.height += 1.5f*fontSize +margin;
        GuiLabel((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width, fontSize }, "Position");

        contentRect.height += fontSize;

        char x_str[16];
        snprintf(x_str, sizeof(x_str), "%0.3f", tf_s1.cord.pos.x);
        GuiTextBox((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width/2.0f -margin, 1.5f*fontSize }, x_str, 20, false);

        char y_str[16];
        snprintf(y_str, sizeof(y_str), "%0.3f", tf_s1.cord.pos.y);
        GuiTextBox((Rectangle){ contentRect.x +settingsRect.width/2.0f, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width/2.0f -margin, 1.5f*fontSize }, y_str, 20, false);
    }

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
