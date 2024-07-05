// TODO: refactor
// - colision check between objects
// - 
//
// TODO: level editor
// TODO: save user levels

#include "asteruga.h"

#define SCREEN_WIDTH()    720
#define SCREEN_HEIGHT()   720

#define BOSS_WON_FADE_TIME 3.0

#define HIT_FULL 1.0

enum boss_state {
    BS_INIT,
    BS_PREPARE_0,
    BS_PREPARE_1,
    BS_PREPARE_2,
    BS_PATTERN_0,
    BS_ROAM,
    BS_WAIT,
    BS_WON,
};

enum level_state {
    LEVEL_START,
    LEVEL_START_1,
    LEVEL_WAVE,
    LEVEL_BOSS,
    LEVEL_END,
    LEVEL_GAME_OVER,
};

struct position2d {
    Vector2 pos;
    Vector2 speed;
    Vector2 accel;
    float angle;
};

enum asteroid_type {
    ASTEROID_BIG,
    ASTEROID_MED,
    ASTEROID_SMALL,
};

enum ascolor {
    ASCOLOR_N,
    ASCOLOR_A,
    ASCOLOR_B,
};

struct bullet {
    struct position2d cord;
    double live_time;
    double live_timestamp;
    bool wraped;
    bool live;
    enum ascolor ascolor;
    float size;
};

struct boss {
    int as_n;
    struct position2d cord;
    struct bullet bullets[200];
    enum ascolor ascolor;
    float hitpoints;
    float hitpoints_max;
    enum boss_state state;
    double state_timestamp;
    double as_timestamp;
    double move_timestamp;
    double shoot_timestamp;
    double fire_rate;
    float bullet_speed;
    int move_n;
    double move_time;
    bool wave_cleared;
    int as_corner;
    bool draw;
    int level;
    float size;
};

struct ship {
    struct position2d cord;
    struct bullet bullets[200];
    double fire_rate;
    double shoot_timestamp;
    double spawn_timestamp;
    enum ascolor ascolor;
    double color_timestamp;
    double ashit_timestamp;
    double bonus_timestamp;
    char *bonus_text;
    int ashit_combo;
    int ashit_combo_max;
    int size;
    int lives;
};

struct asteroid {
    bool draw;
    double spawn_timestamp;
    double spawn_time;
    bool live;
    struct position2d cord;
    int size;
    int type;
    float hitpoints;
    float hitpoints_max;
    int wave;
    enum ascolor ascolor;
};

struct space {
    float top;
    float bot;
    float left;
    float right;
    float width;
    float height;
};

struct asteroid_pack {
    struct asteroid pool[30];
    int size;
    int next_free;
};

struct wave_meta {
    bool spawned;
    bool cleared;
    int asteroids_live;
    double spawn_timestamp;
};

struct wave {
    double spawn_time; 
    const struct asteroid *asteroids;
    int asteroids_n;
};

struct level {
    char *name;
    const struct wave *waves;
    int waves_n;
    double max_time;
    int max_combo;
};

struct color_pad {
    float angle;
    struct position2d cord;
    enum ascolor ascolor;
    float size;
    float rot_dir;
    bool show;
    Vector2 cord_min;
    Vector2 cord_max;
};

struct game {
    double now_timestamp;
    bool paused;
    bool done;
    int score;
    int paused_sel;
    struct ship p1;
    struct boss boss;
    struct asteroid_pack asteroids_pack;
    struct space space;
    struct color_pad color_pad[2];

    double level_timestamp;
    double level_time;
    enum level_state level_state;
    const struct level *levels;
    struct wave_meta waves_meta[100];
    int level;
};

#define ASTEROID_WAVE(pos_x, pos_y, cord_angle, as_type) { .type = ASTEROID_##as_type, .ascolor = ASCOLOR_N, .cord = { .pos = { pos_x, pos_y }, .angle = cord_angle } } 

#define ASTEROID_WAVE_C(pos_x, pos_y, cord_angle, as_type, color_) { .type = ASTEROID_##as_type, .ascolor = color_, .cord = { .pos = { pos_x, pos_y }, .angle = cord_angle } } 

static const struct asteroid wave_1_0[] = { //7
    ASTEROID_WAVE(100, 100, 0, BIG),
};
static const struct asteroid wave_1_1[] = { // 28
    ASTEROID_WAVE(100, 100, (PI/4), BIG),
    ASTEROID_WAVE(100, -100, -(PI/4), BIG),
    ASTEROID_WAVE(-100, -100, -(3*PI/4), BIG),
    ASTEROID_WAVE(-100, 100, (3*PI/4), BIG),
};
static const struct asteroid wave_1_2[] = { //2
    ASTEROID_WAVE(100, 100, (PI/4), SMALL),
    ASTEROID_WAVE(-100, -100, -(3*PI/4), SMALL),
};
static const struct asteroid wave_1_3[] = { //2
    ASTEROID_WAVE(100, -100, -(PI/4), SMALL),
    ASTEROID_WAVE(-100, 100, (3*PI/4), SMALL),
};
static const struct asteroid wave_1_4[] = { //24
    ASTEROID_WAVE(100, 100, 0, MED),
    ASTEROID_WAVE(-100, 200, PI, MED),
    ASTEROID_WAVE(100, 300, 0, MED),
    ASTEROID_WAVE(-100, 400, PI, MED),
    ASTEROID_WAVE(100, -300, 0, MED),
    ASTEROID_WAVE(-100, -400, PI, MED),
    ASTEROID_WAVE(100, -200, 0, MED),
    ASTEROID_WAVE(-100, -100, PI, MED),
};
static const struct asteroid wave_1_5[] = { //12
    ASTEROID_WAVE(100, 100, (PI/4), MED),
    ASTEROID_WAVE(100, -100, -(PI/4), MED),
    ASTEROID_WAVE(-100, -100, -(3*PI/4), MED),
    ASTEROID_WAVE(-100, 100, (3*PI/4), MED),
};
static const struct asteroid wave_1_6[] = { //12
    ASTEROID_WAVE(SCREEN_WIDTH()/2, 50, (PI/2), MED),
    ASTEROID_WAVE(-50, SCREEN_HEIGHT()/2, PI, MED),
    ASTEROID_WAVE(SCREEN_WIDTH()/2, -50, -(PI/2), MED),
    ASTEROID_WAVE(50, SCREEN_HEIGHT()/2, 0, MED),
    ASTEROID_WAVE(SCREEN_WIDTH()/2, 50, (PI/2), MED),
};


#define WAVE(st, aa) { .spawn_time = st, .asteroids = aa, .asteroids_n = sizeof(aa)/sizeof(aa[0]) }

static const struct wave level1_waves[] = {
    WAVE(0, wave_1_0),
    WAVE(20, wave_1_1),
    WAVE(20, wave_1_2),
    WAVE(0.5, wave_1_3),
    WAVE(20, wave_1_4),
    WAVE(20, wave_1_5),
    WAVE(0.5, wave_1_6),
    WAVE(3, wave_1_6),
    WAVE(0.5, wave_1_5),
    WAVE(1, wave_1_5),
    WAVE(0.5, wave_1_6),
};

static const struct asteroid wave_2_0[] = { //7
    ASTEROID_WAVE_C(100, 100, 0, BIG, ASCOLOR_A),
    ASTEROID_WAVE_C(-100, -100, PI, BIG, ASCOLOR_B),
};
static const struct asteroid wave_2_1[] = { //9
    ASTEROID_WAVE_C(30, 30, 0, MED, ASCOLOR_A),
    ASTEROID_WAVE_C(30, SCREEN_HEIGHT()/2, 0, MED, ASCOLOR_A),
    ASTEROID_WAVE_C(30, -30, 0, MED, ASCOLOR_A),
};
static const struct asteroid wave_2_2[] = { //6
    ASTEROID_WAVE_C(-30, 130, PI, MED, ASCOLOR_B),
    ASTEROID_WAVE_C(-30, -130, PI, MED, ASCOLOR_B),
};
static const struct asteroid wave_2_3[] = { //28
    ASTEROID_WAVE(100, 100, (PI/4), BIG),
    ASTEROID_WAVE(100, -100, -(PI/4), BIG),
    ASTEROID_WAVE(-100, -100, -(3*PI/4), BIG),
    ASTEROID_WAVE(-100, 100, (3*PI/4), BIG),
};
static const struct asteroid wave_2_4[] = { //9
    ASTEROID_WAVE((1*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((2*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((3*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((4*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((5*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((6*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((7*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((8*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((9*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
};
static const struct asteroid wave_2_5[] = { //9
    ASTEROID_WAVE(20 +(1*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(2*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(3*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(4*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(5*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(6*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(7*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(8*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(9*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
};
static const struct asteroid wave_2_6[] = { //9
    ASTEROID_WAVE(30, 20 +(1*SCREEN_HEIGHT())/10, 0, SMALL),
    ASTEROID_WAVE(30, 20 +(2*SCREEN_HEIGHT())/10, 0, SMALL),
    ASTEROID_WAVE_C(30, 20 +(5*SCREEN_HEIGHT())/10, 0, SMALL, ASCOLOR_B),
    ASTEROID_WAVE(30, 20 +(4*SCREEN_HEIGHT())/10, 0, SMALL),
    ASTEROID_WAVE_C(30, 20 +(5*SCREEN_HEIGHT())/10, 0, SMALL, ASCOLOR_B),
    ASTEROID_WAVE(30, 20 +(6*SCREEN_HEIGHT())/10, 0, SMALL),
    ASTEROID_WAVE_C(30, 20 +(7*SCREEN_HEIGHT())/10, 0, SMALL, ASCOLOR_B),
    ASTEROID_WAVE(30, 20 +(8*SCREEN_HEIGHT())/10, 0, SMALL),
    ASTEROID_WAVE(30, 20 +(9*SCREEN_HEIGHT())/10, 0, SMALL),
};
static const struct asteroid wave_2_7[] = { //9
    ASTEROID_WAVE(-30, 20 +(1*SCREEN_HEIGHT())/10, PI, SMALL),
    ASTEROID_WAVE(-30, 20 +(2*SCREEN_HEIGHT())/10, PI, SMALL),
    ASTEROID_WAVE_C(-30, 20 +(3*SCREEN_HEIGHT())/10, PI, SMALL, ASCOLOR_A),
    ASTEROID_WAVE(-30, 20 +(4*SCREEN_HEIGHT())/10, PI, SMALL),
    ASTEROID_WAVE_C(-30, 20 +(5*SCREEN_HEIGHT())/10, PI, SMALL, ASCOLOR_A),
    ASTEROID_WAVE(-30, 20 +(6*SCREEN_HEIGHT())/10, PI, SMALL),
    ASTEROID_WAVE_C(-30, 20 +(7*SCREEN_HEIGHT())/10, PI, SMALL, ASCOLOR_A),
    ASTEROID_WAVE(-30, 20 +(8*SCREEN_HEIGHT())/10, PI, SMALL),
    ASTEROID_WAVE(-30, 20 +(9*SCREEN_HEIGHT())/10, PI, SMALL),
};

static const struct wave level2_waves[] = {
    WAVE(0, wave_2_0),
    WAVE(40, wave_2_1),
    WAVE(0.5, wave_2_2),
    WAVE(20, wave_2_3),
    WAVE(2, wave_2_3),
    WAVE(2, wave_2_3),
    WAVE(30, wave_2_4),
    WAVE(3, wave_2_5),
    WAVE(30, wave_2_6),
    WAVE(3, wave_2_7),
};

static const struct asteroid wave_3_0[] = { //28
    ASTEROID_WAVE_C(100, 100, (PI/4), BIG, ASCOLOR_A),
    ASTEROID_WAVE_C(100, -100, -(PI/4), BIG, ASCOLOR_B),
    ASTEROID_WAVE_C(-100, -100, -(3*PI/4), BIG, ASCOLOR_A),
    ASTEROID_WAVE_C(-100, 100, (3*PI/4), BIG, ASCOLOR_B),
};
static const struct asteroid wave_3_1[] = { //28
    ASTEROID_WAVE_C(SCREEN_WIDTH()/2, 100, (PI/2), BIG, ASCOLOR_A),
    ASTEROID_WAVE_C(-100, SCREEN_HEIGHT()/2, PI, BIG, ASCOLOR_B),
    ASTEROID_WAVE_C(SCREEN_WIDTH()/2, -100, -(PI/2), BIG, ASCOLOR_B),
    ASTEROID_WAVE_C(100, SCREEN_HEIGHT()/2, 0, BIG, ASCOLOR_A),
    ASTEROID_WAVE_C(SCREEN_WIDTH()/2, 100, (PI/2), BIG, ASCOLOR_B),
};
static const struct asteroid wave_3_2[] = { //9
    ASTEROID_WAVE((1*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((2*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((3*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((4*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((5*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((6*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((7*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((8*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
    ASTEROID_WAVE((9*SCREEN_WIDTH())/10, 30, (PI/2), SMALL),
};
static const struct asteroid wave_3_3[] = { //9
    ASTEROID_WAVE(20 +(1*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(2*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(3*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(4*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(5*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(6*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(7*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(8*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
    ASTEROID_WAVE(20 +(9*SCREEN_WIDTH())/10, -30, -(PI/2), SMALL),
};

static const struct wave level3_waves[] = {
    WAVE(0,  wave_3_0),
    WAVE(120,  wave_3_1),
    WAVE(120,  wave_3_2),
    WAVE(120,  wave_3_3),
    WAVE(120,  wave_3_2),
    WAVE(3,  wave_3_3),
    WAVE(120,  wave_3_2),
    WAVE(3,  wave_3_3),
    WAVE(3,  wave_3_2),
    WAVE(3,  wave_3_3),
    WAVE(120,  wave_3_2),
    WAVE(3,  wave_3_3),
    WAVE(3,  wave_3_2),
    WAVE(3,  wave_3_3),
    WAVE(300,  wave_3_1),
    WAVE(0.1,  wave_3_2),
    WAVE(2,  wave_3_1),
};

static const struct wave boss_waves[] = {
    WAVE(0,  wave_1_3), // fake wave
};

#define LEVEL(name_, mt, mc, waves_) { .name = name_, .max_time = mt, .max_combo = mc, .waves = waves_, .waves_n = sizeof(waves_)/sizeof(waves_[0]) }

static const struct level levels[4] = {
    LEVEL("level 1", 120, 35, level1_waves),
    LEVEL("level 2", 90, 45,level2_waves),
    LEVEL("level 3", 90, 100, level3_waves),
    LEVEL("BOSS", 300, 60, boss_waves)
};

static struct game game;
static RenderTexture screen_texture;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
#if defined(PLATFORM_WEB)
void call_alert(const char *msg) {
    EM_ASM({
        console.log('I received: ' + UTF8ToString($0));
    }, msg);
    //console.log(msg);
    //console.log(localStorage.a);
    void *localStorage_a = EM_ASM_PTR({
        return stringToNewUTF8(localStorage.a);
    });
    DEBUG_PRINT("###########> 0x%p = %s\n", localStorage_a, (char *)localStorage_a);
}
#endif
#if 0
static float ship_angle_get(struct ship *ship) {
    Vector2 v0 = ship->cord.pos;
    Vector2 v1 = Vector2Add(v0, (Vector2) { 1.0f, 0.0f });
    Vector2 v2 = GetMousePosition();
    return Vector2Angle(Vector2Subtract(v1, v0), Vector2Subtract(v2, v0));
}
#endif

static void asteroid_spawn(struct asteroid_pack *asteroids_pack, struct asteroid *asteroid_ref);

static bool collision_boss_bullet_solve(struct boss *boss, struct bullet *bullet) {
    bool hit = false;
    if (CheckCollisionCircles(boss->cord.pos, boss->size, bullet->cord.pos, bullet->size)) {
        hit = true;
    }
    return hit;
}

static bool collision_ship_bullet_solve(struct ship *ship, struct bullet *bullet) {
    bool hit = false;
    if (CheckCollisionCircles(ship->cord.pos, ship->size, bullet->cord.pos, bullet->size)) {
        hit = true;
    }
    return hit;
}

static bool collision_boss_ship_solve(struct boss *boss, struct ship *ship) {
    bool hit = false;
    if (CheckCollisionCircles(boss->cord.pos, boss->size, ship->cord.pos, ship->size)) {
        hit = true;
    }
    return hit;
}

static void level_waves_clear(void) {
    for (int i = 0; i < 100; i++) {
        memset(&game.waves_meta[i], 0, sizeof(struct wave_meta));
    }
}


static double GetTimeUnpaused(void) {
    return game.now_timestamp;
}

static Color color_get(enum ascolor ascolor) {
    Color color;
    if (ascolor == ASCOLOR_A) {
        color = COLOR_A;
    } else if (ascolor == ASCOLOR_B) {
        color = COLOR_B;
    } else {
        color = COLOR_NEUTRAL;
    }
    return color;
}

static void level_wave_update(void) {
    // check all waves
    const struct level *level = &game.levels[game.level];
    int waves_cleared = 0;
    for (int i = 0; i < level->waves_n; i++) {
        const struct wave *wave = &level->waves[i];
        struct wave_meta *wave_meta = &game.waves_meta[i];
        struct wave_meta *wave_meta_dep = NULL;
        //DEBUG_PRINT("%i:%i wave check\n", game.level, i);
        if (wave_meta->cleared) {
            waves_cleared++;
            continue;
        }
        if (wave_meta->spawned) {
            //DEBUG_PRINT("%i:%i wave spawned already\n", game.level, i);
            continue;
        }

        double prev_wave_spawn_timestamp = game.level_timestamp;
        bool spawned = true;
        if (i > 0) {
            wave_meta_dep = &game.waves_meta[i -1];
            prev_wave_spawn_timestamp = wave_meta_dep->spawn_timestamp;
            spawned = wave_meta_dep->spawned;
        }

        // spawn by time
        if ((GetTimeUnpaused() -prev_wave_spawn_timestamp) > wave->spawn_time && spawned) {
            DEBUG_PRINT("wave %i spawn by time c:%2.f p:%.2f\n", i, GetTimeUnpaused(), prev_wave_spawn_timestamp);
            for (int j = 0; j < wave->asteroids_n; j++) {
                struct asteroid asteroid = wave->asteroids[j];
                asteroid.wave = i;
                asteroid.spawn_time = 2.0;
                asteroid_spawn(&game.asteroids_pack, &asteroid);
            }
            wave_meta->spawned = true;
            wave_meta->spawn_timestamp = GetTimeUnpaused();
        }

        // spawn by wave dep
        if (wave_meta_dep == NULL) {
            continue;
        }
        if (wave_meta_dep->cleared == true) {
            DEBUG_PRINT("wave %i spawn by dep\n", i);
            for (int j = 0; j < wave->asteroids_n; j++) {
                struct asteroid asteroid = wave->asteroids[j];
                asteroid.wave = i;
                asteroid.spawn_time = 2.0;
                asteroid_spawn(&game.asteroids_pack, &asteroid);
            }
            wave_meta->spawned = true;
            wave_meta->spawn_timestamp = GetTimeUnpaused();
        }
    }
    if (waves_cleared == level->waves_n) {
        game.level_state = LEVEL_END;
    }
}

static bool space_object_wrap_around(struct space *space, Vector2 *obj_pos) {
    bool wraped = false;
    // wrap ships position into space
    int mod_x = (int)obj_pos->x % (int)space->width;
    int mod_y = (int)obj_pos->y % (int)space->height;
    if (obj_pos->y > space->bot) {
        obj_pos->y = space->top + mod_y;
        wraped = true;
    }
    if (obj_pos->y < space->top) {
        obj_pos->y = space->bot + mod_y;
        wraped = true;
    }
    if (obj_pos->x > space->right) {
        obj_pos->x = space->left + mod_x;
        wraped = true;
    }
    if (obj_pos->x < space->left) {
        obj_pos->x = space->right + mod_x;
        wraped = true;
    }
    return wraped;
}

static void ship_gas(struct ship *ship, float dir) {
    float accel_mod = 0.1f*dir;
    Vector2 accel = { accel_mod*cosf(ship->cord.angle), accel_mod*sinf(ship->cord.angle) };

    ship->cord.speed = Vector2Add(ship->cord.speed, accel);
}

static void ship_strafe_right(struct ship *ship) {
    float accel_mod = 0.1f;
    Vector2 accel = { accel_mod*cosf(ship->cord.angle + (PI/2)), accel_mod*sinf(ship->cord.angle + (PI/2)) };

    ship->cord.speed = Vector2Add(ship->cord.speed, accel);
}

static void ship_strafe_left(struct ship *ship) {
    float accel_mod = 0.1f;
    Vector2 accel = { accel_mod*cosf(ship->cord.angle - (PI/2)), accel_mod*sinf(ship->cord.angle - (PI/2)) };

    ship->cord.speed = Vector2Add(ship->cord.speed, accel);
}

static void ship_turn(struct ship *ship, float dir) {
    ship->cord.angle += ((PI*dir)/360.0f);
}

static void ship_reset(struct ship *ship) {
    ship->bonus_timestamp = GetTimeUnpaused() -10;
    ship->ashit_combo = 0;
    ship->fire_rate = 0.15;
    ship->cord.pos = (Vector2){ (game.space.right - game.space.left)/2.0f, (game.space.bot -game.space.top)/2.0f };
    ship->cord.speed = Vector2Zero();
    ship->cord.angle = -PI/2.0f;
    ship->shoot_timestamp = GetTimeUnpaused();
    ship->spawn_timestamp = GetTimeUnpaused();
    ship->ascolor = ASCOLOR_N;
    ship->color_timestamp = GetTimeUnpaused();
}

static void ship_destroy(struct game_context *gctx, struct ship *ship) {
    game_sfx_play(gctx, SFX_SHIP_EXPLOSION);

    ship->lives--;
    if (ship->lives <= 0) {
        game.level_state = LEVEL_END;
    }
}

static void boss_shoot(struct game_context *gctx, struct boss *boss) {
    if ((GetTimeUnpaused() -boss->shoot_timestamp) < boss->fire_rate) {
        goto out;
    }
    boss->shoot_timestamp = GetTimeUnpaused();

    int bullets_total = sizeof(boss->bullets)/sizeof(struct bullet);
    struct bullet *bullet = NULL;
    for (int i = 0; i < bullets_total; i++) {
        if (!boss->bullets[i].live) {
            bullet = &boss->bullets[i];
            break;
        }
    }
    if (bullet == NULL) {
        perror("no more bullets");
        goto out;
    }

    float angle = GetRandomValue(0,360)*DEG2RAD;
    float sinres = sinf(angle);
    float cosres = cosf(angle);

    bullet->ascolor = boss->ascolor;
    bullet->cord.pos = boss->cord.pos;
    bullet->cord.speed.x = boss->bullet_speed*cosres;
    bullet->cord.speed.y = boss->bullet_speed*sinres;
    bullet->live = true;
    bullet->size = 3.0f;
    bullet->live_timestamp = GetTimeUnpaused() + 20;
    bullet->live_time = 0.5;
    bullet->wraped = false;

    game_sfx_play(gctx, SFX_SHIP_SHOOT);

out:
    return;
}

static void ship_shoot(struct game_context *gctx, struct ship *ship) {
    if ((GetTimeUnpaused() -ship->shoot_timestamp) < ship->fire_rate) {
        goto out;
    }
    ship->shoot_timestamp = GetTimeUnpaused();

    int bullets_total = sizeof(ship->bullets)/sizeof(struct bullet);
    struct bullet *bullet = NULL;
    for (int i = 0; i < bullets_total; i++) {
        if (!ship->bullets[i].live) {
            bullet = &ship->bullets[i];
            break;
        }
    }
    if (bullet == NULL) {
        perror("no more bullets");
        goto out;
    }

    float sinres = sinf(ship->cord.angle);
    float cosres = cosf(ship->cord.angle);

    bullet->ascolor = ship->ascolor;
    bullet->cord.pos = ship->cord.pos;
    bullet->cord.speed.x = 10.0f*cosres;
    bullet->cord.speed.y = 10.0f*sinres;
    bullet->live = true;
    bullet->size = 3.0f;
    bullet->live_timestamp = GetTimeUnpaused() + 20;
    bullet->live_time = 0.5;
    bullet->wraped = false;

    game_sfx_play(gctx, SFX_SHIP_SHOOT);

out:
    return;
}

static void bullets_draw(struct bullet *bullets, int size) {
    for (int i = 0; i < size; i++) {
        struct bullet *bullet = &bullets[i];
        if (!bullet->live) {
            continue;
        }
        Color color = color_get(bullet->ascolor);
        DrawCircle(bullet->cord.pos.x, bullet->cord.pos.y, bullet->size, color);
    }
}

static void ship_draw(struct ship *ship) {

#if 0
    Vector2 v0 = ship->cord.pos;
    Vector2 v1 = Vector2Add(v0, (Vector2) { 1.0f, 0.0f });
    Vector2 v2 = GetMousePosition();
    angle = Vector2Angle(Vector2Subtract(v1, v0), Vector2Subtract(v2, v0));
#endif

    Vector2 s0 = { ship->cord.pos.x +ship->size, ship->cord.pos.y };
    Vector2 s1 = { ship->cord.pos.x -ship->size, ship->cord.pos.y -ship->size/2.0f};
    Vector2 s2 = { ship->cord.pos.x -ship->size, ship->cord.pos.y +ship->size/2.0f};

    Matrix mr, mt;
    Vector2 s0r = s0, s1r = s1, s2r = s2;
    Vector2 v0 = ship->cord.pos;
    mr = MatrixRotateXYZ((Vector3) { 0.0f, 0.0f, ship->cord.angle });
    mt = MatrixTranslate(-v0.x, -v0.y, 0.0f );
    s0r = Vector2Transform(s0r, mt);
    s1r = Vector2Transform(s1r, mt);
    s2r = Vector2Transform(s2r, mt);
    s0r = Vector2Transform(s0r, mr);
    s1r = Vector2Transform(s1r, mr);
    s2r = Vector2Transform(s2r, mr);
    mt = MatrixTranslate(v0.x, v0.y, 0.0f );
    s0r = Vector2Transform(s0r, mt);
    s1r = Vector2Transform(s1r, mt);
    s2r = Vector2Transform(s2r, mt);

    Color color = color_get(ship->ascolor);
    // blinks color in the last 3s
    if (ship->ascolor != ASCOLOR_N) {
        double tdiff = GetTimeUnpaused() -ship->color_timestamp;
        if (tdiff > 7.0) {
            if ((int)(tdiff*20) % 2 == 0) {
                color = COLOR_NEUTRAL;
            }
        }
    }

    double tdiff_alive = GetTimeUnpaused() -ship->spawn_timestamp;
    if (tdiff_alive < 2.0) {
        if ((int)(tdiff_alive*20) % 2 == 0) {
            color = RAYWHITE;
        }
    }
    DrawTriangle(s0r, s1r, s2r, color);
    DrawLineEx(s0r, s1r, 3.0, COLOR_NEUTRAL);
    DrawLineEx(s1r, s2r, 3.0, COLOR_NEUTRAL);
    DrawLineEx(s2r, s0r, 3.0, COLOR_NEUTRAL);

    double btdiff = (GetTimeUnpaused() -ship->bonus_timestamp);
    if (btdiff < 3.0) {
        float fsize = 10 + 20*btdiff/1.0;
        if (fsize > 30) {
            fsize = 30;
        }
        DrawTextCentered(ship->bonus_text, ship->cord.pos.x+2, ship->cord.pos.y+2, fsize, COLOR_A);
        DrawTextCentered(ship->bonus_text, ship->cord.pos.x+4, ship->cord.pos.y+4, fsize, COLOR_B);
        DrawTextCentered(ship->bonus_text, ship->cord.pos.x, ship->cord.pos.y, fsize, COLOR_NEUTRAL);
    }

#if 0
    for (int i = 0; i < ship->lives; i++) {
        Vector2 live_pos = { 2*ship->size*i + 30.0f, ship->size + 10.0f };
        DrawPoly(live_pos, 3, ship->size, -90, COLOR_NEUTRAL);
    }
#endif
}

static void bullet_update(struct bullet *bullet) {
    if (!bullet->live) {
        return;
    }

    if (bullet->wraped && (GetTimeUnpaused() -bullet->live_timestamp) > bullet->live_time) {
        memset(bullet, 0, sizeof(struct bullet));
        return;
    }

    bullet->cord.pos = Vector2Add(bullet->cord.pos, bullet->cord.speed);

    if (space_object_wrap_around(&game.space, &bullet->cord.pos)) {
        bullet->live_timestamp = GetTimeUnpaused();
        bullet->wraped = true;
    }
}

bool collision_bullet_asteroid_solve(struct bullet *bullet, struct asteroid *asteroid) {
    bool hit = false;
    if (!bullet->live || !asteroid->live) {
        return false;
    }
    if ((GetTimeUnpaused() -asteroid->spawn_timestamp) < asteroid->spawn_time) {
        return false;
    }
    if (CheckCollisionCircles(bullet->cord.pos, bullet->size, asteroid->cord.pos, asteroid->size)) {
        hit = true;
    }
    return hit;
}

bool collision_ship_asteroid_solve(struct ship *ship, struct asteroid *asteroid) {
    bool hit = false;
    if (!asteroid->live) {
        return false;
    }
    if ((GetTimeUnpaused() -asteroid->spawn_timestamp) < asteroid->spawn_time) {
        return false;
    }
    if (CheckCollisionCircles(ship->cord.pos, ship->size, asteroid->cord.pos, asteroid->size)) {
        hit = true;
    }
    return hit;
}

bool collision_ship_color_pad_solve(struct ship *ship, struct color_pad *color_pad) {
    bool hit = false;
    if (!color_pad->show) {
        goto out;
    }
    if (CheckCollisionCircles(ship->cord.pos, ship->size, color_pad->cord.pos, color_pad->size)) {
        hit = true;
    }
out:
    return hit;
}

void ship_color(struct ship *ship, enum ascolor ascolor) {
    ship->ascolor = ascolor;
    ship->color_timestamp = GetTimeUnpaused();
}

struct asteroid *asteroid_get_free(struct asteroid_pack *asteroids_pack) {
    struct asteroid *asteroid = NULL;

    for (int searched = 0; searched < asteroids_pack->size; searched++) {
        if (!asteroids_pack->pool[asteroids_pack->next_free].live) {
            asteroid = &asteroids_pack->pool[asteroids_pack->next_free];

            searched = asteroids_pack->size; // break on next loop
        }
        asteroids_pack->next_free++;
        asteroids_pack->next_free%=asteroids_pack->size;
    }

    if (asteroid == NULL) {
        DEBUG_PRINT("NO MORE SPACE FOR ASTEROIDS!\n");
    }
    return asteroid;
}

static void asteroid_spawn(struct asteroid_pack *asteroids_pack, struct asteroid *asteroid_ref) {
    struct asteroid *asteroid = asteroid_get_free(asteroids_pack);
    if (asteroid == NULL) {
        return;
    }
    *asteroid = *asteroid_ref;
    if (asteroid->wave >= 0) {
        game.waves_meta[asteroid->wave].asteroids_live++;
        game.waves_meta[asteroid->wave].cleared = false;
    }

    asteroid->live = true;
    float speed_mod;
    if (asteroid->type == ASTEROID_BIG) {
        asteroid->size = 50;
        speed_mod = 1.0f;
        asteroid->hitpoints_max = 7.0;
    } else if (asteroid->type == ASTEROID_MED) {
        asteroid->size = 30;
        speed_mod = 3.0f;
        asteroid->hitpoints_max = 5.0;
    } else { // ASTEROID_SMALL
        asteroid->size = 10;
        speed_mod = 5.0f;
        asteroid->hitpoints_max = 1.0;
    }
    asteroid->hitpoints = asteroid->hitpoints_max;

    float sinres = sinf(asteroid->cord.angle);
    float cosres = cosf(asteroid->cord.angle);
    asteroid->cord.speed = (Vector2){ speed_mod*cosres, speed_mod*sinres };
    asteroid->spawn_timestamp = GetTimeUnpaused();
}

static void asteroid_destroy(struct asteroid *asteroid) {
    game.waves_meta[asteroid->wave].asteroids_live--;
    if (game.waves_meta[asteroid->wave].asteroids_live <= 0) {
        game.waves_meta[asteroid->wave].cleared = true;
    }

    memset(asteroid, 0, sizeof(struct asteroid));
}


static void asteroid_update(struct asteroid_pack *asteroids_pack, struct space *space) {
    for (int i = 0; i < asteroids_pack->size; i++) {
        struct asteroid *asteroid = &asteroids_pack->pool[i];
        if (!asteroid->live) {
            continue;
        }

        space_object_wrap_around(space, &asteroid->cord.pos);

        if ((GetTimeUnpaused() -asteroid->spawn_timestamp) < asteroid->spawn_time) {
            continue;
        }

        asteroid->cord.pos = Vector2Add(asteroid->cord.pos, asteroid->cord.speed);
    }
}

static void color_pad_show(struct color_pad *color_pad) {
    if (color_pad->show) {
        return;
    }

    color_pad->cord.pos.x = GetRandomValue(color_pad->cord_min.x +(int)color_pad->size, color_pad->cord_max.x -(int)color_pad->size);
    color_pad->cord.pos.y = GetRandomValue(color_pad->cord_min.y +(int)color_pad->size, color_pad->cord_max.y -(int)color_pad->size);
    color_pad->show = true;
    color_pad->angle = 0.0f;
}

static void color_pad_hide(struct color_pad *color_pad) {
    color_pad->show = false;
}


static void asteroid_draw(struct asteroid_pack *asteroids_pack) {
    int n_color[3] = { 0, 0, 0 };
    for (int i = 0; i < asteroids_pack->size; i++) {
        struct asteroid *asteroid = &asteroids_pack->pool[i];
        if (asteroid->live) {
            Color color = color_get(asteroid->ascolor);
            if (asteroid->ascolor == ASCOLOR_A) {
                n_color[asteroid->ascolor]++;
            } else if (asteroid->ascolor == ASCOLOR_B) {
                n_color[asteroid->ascolor]++;
            }
            if ((GetTimeUnpaused() -asteroid->spawn_timestamp) < asteroid->spawn_time) {
                asteroid->draw = !asteroid->draw;
            } else {
                asteroid->draw = true;
            }
            if (asteroid->draw) {
                DrawCircle(asteroid->cord.pos.x, asteroid->cord.pos.y, asteroid->size, color);
                DrawCircle(asteroid->cord.pos.x, asteroid->cord.pos.y, (asteroid->size -3.0f) * ((float)(asteroid->hitpoints_max -asteroid->hitpoints)/asteroid->hitpoints_max), COLOR_BACKGROUND);
            }
        }
    }
    if (n_color[ASCOLOR_A] > 0 && game.p1.ascolor != ASCOLOR_A) {
        color_pad_show(&game.color_pad[0]);
    } else {
        color_pad_hide(&game.color_pad[0]);
    }
    if (n_color[ASCOLOR_B] > 0 && game.p1.ascolor != ASCOLOR_B) {
        color_pad_show(&game.color_pad[1]);
    } else {
        color_pad_hide(&game.color_pad[1]);
    }
}

static void asteroids_new(struct asteroid_pack *asteroids_pack) {
    memset(asteroids_pack->pool, 0, sizeof(struct asteroid)*30);
    asteroids_pack->size = 30;
    asteroids_pack->next_free = 0;
}

static void game_init(struct game_context *gctx) {
    memset(&game, 0, sizeof(struct asteroid));
}

static void game_exit(struct game_context *gctx) {
    game_theme_stop(gctx);
}

static void game_enter(struct game_context *gctx) {
    game.now_timestamp = GetTime();
    game.done = false;
    game.score = 0;
    game.paused = false;
    game.paused_sel = 0;

    // levels initialization
    game.level = gctx->level_sel;
    if (game.level == 0) {
        game_theme_play(gctx, THEME_LEVEL2);
    } else if (game.level == 1) {
        game_theme_play(gctx, THEME_LEVEL2);
    } else if (game.level == 2) {
        game_theme_play(gctx, THEME_LEVEL2);
    } else if (game.level == 3) {
        game_theme_play(gctx, THEME_BOSS);
    }
    DEBUG_PRINT("level_sel=%i\n", game.level);
    game.level_state = LEVEL_START;
    game.levels = levels;
    // space limits
    game.space.width = SCREEN_WIDTH();
    game.space.height = SCREEN_HEIGHT();
    game.space.left = 0;
    game.space.right = SCREEN_WIDTH();
    game.space.top = 0;
    game.space.bot = SCREEN_HEIGHT();
    screen_texture = LoadRenderTexture(SCREEN_WIDTH(), SCREEN_HEIGHT());
}

#if 0
enum ctrl_input {
    SHIP_GAS,
    SHIP_ROT_CW,
    SHIP_ROT_CC,
    SHIP_STRAFE_LEFT,
    SHIP_STRAFE_RIGHT,
    SHIP_SHOOT,
};
#endif

static void ctrl_input_mapping(struct game_context *gctx) {
    if (IsKeyDown(KEY_SPACE) ||
        IsKeyDown(KEY_ENTER)) {
        ship_shoot(gctx, &game.p1);
    }
    if (IsKeyDown(KEY_W)) {
        ship_gas(&game.p1, 1.0f);
    }
    if (IsKeyDown(KEY_S)) {
        ship_gas(&game.p1, -1.0f);
    }
    if (IsKeyDown(KEY_A)) {
        ship_turn(&game.p1, -5.0f);
    }
    if (IsKeyDown(KEY_D)) {
        ship_turn(&game.p1, 5.0f);
    }
    if (IsKeyDown(KEY_Q)) {
        ship_strafe_left(&game.p1);
    }
    if (IsKeyDown(KEY_E)) {
        ship_strafe_right(&game.p1);
    }
}

static float deaccel(float speed) {
    float sign = 1.0f;

    if (speed < 0.0f) {
        sign = -1.0f;
    }
    float res = fabs(speed) -0.01f;
    if (res < 0.0f) {
        res = 0.0f;
    }
    return res*sign;
}

static void ship_apply_bonus(struct ship *ship) {
    if (ship->ashit_combo == 10) { 
        ship->fire_rate *= 0.8;
        ship->ashit_combo++;
        ship->bonus_text = "FIRE RATE UP";
        ship->bonus_timestamp = GetTimeUnpaused();
    } else if (ship->ashit_combo == 20) { 
        ship->fire_rate *= 0.8;
        ship->ashit_combo++;
        ship->bonus_text = "FIRE RATE UP";
        ship->bonus_timestamp = GetTimeUnpaused();
    } else if (ship->ashit_combo == 30) { 
        ship->fire_rate *= 0.8;
        ship->ashit_combo++;
        ship->bonus_text = "FIRE RATE UP";
        ship->bonus_timestamp = GetTimeUnpaused();
    } else if (ship->ashit_combo == 40) { 
        ship->fire_rate *= 0.8;
        ship->ashit_combo++;
        ship->bonus_text = "FIRE RATE UP";
        ship->bonus_timestamp = GetTimeUnpaused();
    } else if (ship->ashit_combo == 50) { 
        ship->lives++;
        ship->ashit_combo++;
        ship->bonus_text = "+1 LIFE";
        ship->bonus_timestamp = GetTimeUnpaused();
    }
}

static void ship_update(struct ship *ship) {
    ship->cord.pos = Vector2Add(ship->cord.pos, ship->cord.speed); 

    // de-accel ship
    ship->cord.speed.x = deaccel(ship->cord.speed.x);
    ship->cord.speed.y = deaccel(ship->cord.speed.y);

    if ((GetTimeUnpaused() -ship->ashit_timestamp) > 20.0) {
        ship->ashit_combo = 0;
        ship->fire_rate = 0.15;
    }
    if ((GetTimeUnpaused() -ship->color_timestamp) > 10.0) {
        ship_color(&game.p1, ASCOLOR_N);
    }
}

static void bullet_destroy(struct bullet *bullet) {
    memset(bullet, 0, sizeof(struct bullet));
}

static void color_pad_update(struct color_pad *color_pad) {
    if (!color_pad->show) {
        return;
    }

    color_pad->angle += 3.0f*color_pad->rot_dir;
}

static void boss_update(struct game_context *gctx, struct boss *boss, struct ship *ship) {
    if (boss->state == BS_INIT) {
        boss->cord.pos = (Vector2){ SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -100 };
        boss->state = BS_PREPARE_0;
        boss->draw = false;
        boss->level = 0;
        boss->size = 40;
        boss->state_timestamp = GetTimeUnpaused();
        boss->move_time = GetRandomValue(0, 20)*0.1;
        boss->wave_cleared = false;
        boss->hitpoints_max = 10;
        boss->hitpoints = boss->hitpoints_max;

        ship->spawn_timestamp = GetTimeUnpaused() -3.0;
        ship->cord.pos.y += 100;
        level_waves_clear();
    } else if (boss->state == BS_PREPARE_0) {
        boss->cord.pos = (Vector2){ SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -100 };
        boss->draw = !boss->draw;
        if ((GetTimeUnpaused() -boss->state_timestamp) > 1.0) {
            if (boss->wave_cleared) {
                boss->wave_cleared = false;
                boss->state = BS_ROAM;
            } else {
                boss->state = BS_PREPARE_1;
            }
            boss->state_timestamp = GetTimeUnpaused();
        }
    } else if (boss->state == BS_PREPARE_1) {
        boss->cord.pos = (Vector2){ SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -100 };
        boss->draw = true;
        if ((GetTimeUnpaused() -boss->state_timestamp) > 2.0) {
            boss->state = BS_PREPARE_2;
            boss->state_timestamp = GetTimeUnpaused();
        }
    } else if (boss->state == BS_PREPARE_2) {
        boss->cord.pos = (Vector2){ SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -100 };
        boss->draw = !boss->draw;
        if ((GetTimeUnpaused() -boss->state_timestamp) > 1.0) {
            boss->draw = false;
            boss->hitpoints_max = 30 +30*boss->level;
            boss->hitpoints = boss->hitpoints_max;
            boss->state = BS_PATTERN_0;
            boss->state_timestamp = GetTimeUnpaused();
            boss->as_timestamp = GetTimeUnpaused();
            boss->as_corner = 0;
            boss->as_n = 4*2;
            boss->fire_rate = 0.5 -0.08*boss->level;
            boss->bullet_speed = 3.0 +2*boss->level;
            level_waves_clear();
        }
    } else if (boss->state == BS_PATTERN_0) {
        // spawn medium asteroid on each corner once every 1s
        float shoot_time = 1.0 -boss->level*0.25;
        if (boss->as_corner > 3) {
            boss->as_corner = 0;
        }

        float angle;
        struct asteroid asteroid;
        if (boss->as_corner == 0) {
            angle = GetRandomValue(10, 80)*DEG2RAD;
            asteroid = (struct asteroid)ASTEROID_WAVE(100, 100, angle, MED);
        } else if (boss->as_corner == 1) {
            angle = GetRandomValue(100, 170)*DEG2RAD;
            asteroid = (struct asteroid)ASTEROID_WAVE(-100, 100, angle, MED);
        } else if (boss->as_corner == 2) {
            angle = GetRandomValue(190, 260)*DEG2RAD;
            asteroid = (struct asteroid)ASTEROID_WAVE(-100, -100, angle, MED);
        } else if (boss->as_corner == 3) {
            angle = GetRandomValue(280, 350)*DEG2RAD;
            asteroid = (struct asteroid)ASTEROID_WAVE(100, -100, angle, MED);
        }

        if ((GetTimeUnpaused() -boss->as_timestamp) > shoot_time) {
            for (int i = 0; i < boss->level +1; i++) {
                asteroid.wave = 0;
                asteroid.spawn_time = 0.0;
                int rancolor = GetRandomValue(0, 10);
                if ((boss->level == 1 && rancolor <= 3) ||
                    (boss->level == 2 && rancolor <= 5)) {
                    if (rancolor % 2 == 0) {
                        asteroid.ascolor = ASCOLOR_A;
                    } else {
                        asteroid.ascolor = ASCOLOR_B;
                    }
                }
                asteroid_spawn(&game.asteroids_pack, &asteroid);
                asteroid.cord.angle += GetRandomValue(-15,15)*DEG2RAD;
            }

            boss->as_timestamp = GetTimeUnpaused();
            boss->as_corner++;
            boss->as_n--;
        }
        if (boss->as_n <= 0) {
            boss->state = BS_WAIT;
            boss->state_timestamp = GetTimeUnpaused();
        }
    } else if (boss->state == BS_ROAM) {
        boss->draw = true;

        boss->cord.pos = Vector2Add(boss->cord.pos, boss->cord.speed); 

        // de-accel boss
        boss->cord.speed.x = deaccel(boss->cord.speed.x);
        boss->cord.speed.y = deaccel(boss->cord.speed.y);
        space_object_wrap_around(&game.space, &game.boss.cord.pos);

        if (collision_boss_ship_solve(boss, ship) &&
            (GetTimeUnpaused() -ship->spawn_timestamp) > 2.0) {
            ship_destroy(gctx, ship);
            ship_reset(ship);
        }
        int bullets_total = sizeof(ship->bullets)/sizeof(struct bullet);
        for (int b = 0; b < bullets_total; b++) {
            struct bullet *bullet = &ship->bullets[b];
            if (!bullet->live) {
                continue;
            }
            if (collision_boss_bullet_solve(boss, bullet)) {
                DEBUG_PRINT("BOSS HIT");
                if (boss->ascolor == ASCOLOR_N || bullet->ascolor == boss->ascolor) {
                    // full hit
                    boss->hitpoints -= 1.0f;
                    game_sfx_play(gctx, SFX_ASTEROID_HIT_PLUS);
                } else {
                    boss->hitpoints -= 0.1f;
                    game_sfx_play(gctx, SFX_ASTEROID_HIT);
                }
                bullet_destroy(bullet);
            }
        }

        for (int i = 0; i < boss->level +1; i++) {
            boss_shoot(gctx, &game.boss);
        }

        int bullets_total2 = sizeof(boss->bullets)/sizeof(struct bullet);
        for (int b = 0; b < bullets_total2; b++) {
            struct bullet *bullet = &boss->bullets[b];
            if (!bullet->live) {
                continue;
            }
            if (collision_ship_bullet_solve(ship, bullet) &&
                (GetTimeUnpaused() -ship->spawn_timestamp) > 2.0) {
                ship_destroy(gctx, ship);
                ship_reset(ship);
                bullet_destroy(bullet);
            }
        }

        if ((GetTimeUnpaused() -boss->move_timestamp) > boss->move_time) {
            boss->cord.speed.x = GetRandomValue(-10, 10)*(0.5 +boss->level*0.08);
            boss->cord.speed.y = GetRandomValue(-10, 10)*(0.5 +boss->level*0.08);

            boss->move_timestamp = GetTimeUnpaused();
            boss->move_time = 2;//GetRandomValue(0, 20)*0.1;
            boss->move_n--;
        }
        if (boss->hitpoints < 0.0f) {
            boss->level++;
            boss->move_n = 0;
        }
        if (boss->move_n <= 0) {
            if (boss->level > 2) {
                DEBUG_PRINT("boss dead");
                boss->state = BS_WON;
                game_sfx_play(gctx, SFX_BOSS_DEFEAT);
                game_theme_stop(gctx);
            } else {
                DEBUG_PRINT("boss next");
                boss->state = BS_PREPARE_0;
                boss->hitpoints = boss->hitpoints_max;
            }
            boss->state_timestamp = GetTimeUnpaused();
            boss->wave_cleared = false;
        }
    } else if (boss->state == BS_WAIT) {
        if (game.waves_meta[0].cleared) {
            level_waves_clear();
            boss->state = BS_PREPARE_0;
            boss->state_timestamp = GetTimeUnpaused();
            boss->wave_cleared = true;

            boss->move_time = 2.0;//GetRandomValue(0, 20)*0.1;
            boss->move_timestamp = GetTimeUnpaused() -boss->move_time;
            boss->move_n = 10 -boss->level*2;
        }
    } else if (boss->state == BS_WON) {
        boss->cord.pos = (Vector2){ SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -100 };
        boss->draw = !boss->draw;

        if ((GetTimeUnpaused() -boss->state_timestamp) > BOSS_WON_FADE_TIME) {
            game.level_state = LEVEL_END;
        }
    }
}

static void boss_draw(struct boss *boss) {
    if (boss->draw) {
        Color color = color_get(boss->ascolor);
        DrawCircle(boss->cord.pos.x, boss->cord.pos.y, boss->size, color);
        DrawCircle(boss->cord.pos.x, boss->cord.pos.y, (boss->size -3.0f) * ((float)(boss->hitpoints_max -boss->hitpoints)/boss->hitpoints_max), COLOR_BACKGROUND);
    }

    if (boss->state == BS_WON) {
        double tdiff = GetTimeUnpaused() -boss->state_timestamp;
        DrawRectangle(0, 0, SCREEN_WIDTH(), SCREEN_HEIGHT(), Fade(COLOR_BACKGROUND, tdiff/BOSS_WON_FADE_TIME));
    }
}

static void game_update(struct game_context *gctx) {
    game_theme_update(gctx);
    if (IsKeyPressed(KEY_ESCAPE) && !game.done) {
        //DEBUG_PRINT("ESCAPE PRESS");
        //paused = false;
        game.paused = !game.paused;
        if (game.paused) {
            game.paused_sel = 0;
        }
    }
    if (game.paused || game.done) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            game.paused_sel--;
            if (game.paused_sel < 0) {
                game.paused_sel = 2;
            }
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            game.paused_sel++;
            if (game.paused_sel > 2) {
                game.paused_sel = 0;
            }
        }
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            if (game.paused_sel == 0 && !game.done) {
                game.paused = false;
            } else if (game.paused_sel == 0 && game.done) {
                game_score_push(game.level, game.score);
                game_state_change(gctx, STATE_MAIN_MENU);
            } else if (game.paused_sel == 1) {
                game.level_state = LEVEL_START;
                game.paused = false;
                game.done = false;
            } else if (game.paused_sel == 2) {
                game_state_change(gctx, STATE_MAIN_MENU);
            }
        }
        return;
    }
    game.now_timestamp += GetFrameTime();
    if (game.level_state == LEVEL_START) {
        game.level_timestamp = GetTimeUnpaused();
        game.level_state = LEVEL_START_1;
        // TODO function to reset ships position
        //

        game.boss.state = BS_INIT;
        game_theme_restart(gctx);

        ship_reset(&game.p1);
        game.p1.size = 25;
        game.p1.ascolor = ASCOLOR_N;
        game.p1.fire_rate = 0.15;
        game.p1.lives = 3;
        game.p1.ashit_combo = 0;
        game.p1.ashit_combo_max = 0;

        game.color_pad[0].ascolor = ASCOLOR_A;
        game.color_pad[0].size = 20.0;
        game.color_pad[0].rot_dir = 1.0;
        game.color_pad[0].cord.pos = (Vector2){ 300.0, 300.0 };
        game.color_pad[0].show = false;
        game.color_pad[0].cord_min = (Vector2){ game.space.left, game.space.top };
        game.color_pad[0].cord_max = (Vector2){ game.space.width/2, game.space.bot };
        game.color_pad[1].ascolor = ASCOLOR_B;
        game.color_pad[1].size = 20.0;
        game.color_pad[1].rot_dir = -1.0;
        game.color_pad[1].cord.pos = (Vector2){ 350.0, 300.0 };
        game.color_pad[1].show = false;
        game.color_pad[1].cord_min = (Vector2){ game.space.width/2, game.space.top };
        game.color_pad[1].cord_max = (Vector2){ game.space.right, game.space.bot };

        game.level_time = 999999;
        game.score = 0;
        game.done = false;
        game.paused = false;

        asteroids_new(&game.asteroids_pack);
    } else if (game.level_state == LEVEL_START_1) {
        if (0.10 < (GetTimeUnpaused() -game.level_timestamp)) {
            game.level_timestamp = GetTimeUnpaused();
            if (game.level == 3) {
                game.level_state = LEVEL_BOSS;
            } else {
                game.level_state = LEVEL_WAVE;
            }
            level_waves_clear();
            ship_reset(&game.p1);
        }
    } else if (game.level_state == LEVEL_END) {
        game.paused_sel = 0;
        game.done = true;
        // scoring
        if (game.p1.lives >= 3) {
            game.score++;
        }
        const struct level *level = &game.levels[game.level];
        if (game.p1.ashit_combo_max >= level->max_combo) {
            game.score++;
        }
        game.level_time = GetTimeUnpaused() -game.level_timestamp;
        if (game.level_time < level->max_time) {
            game.score++;
        }
        game.level_state = LEVEL_GAME_OVER;
    } else if (game.level_state == LEVEL_BOSS) {
        boss_update(gctx, &game.boss, &game.p1);
    } else if (game.level_state == LEVEL_WAVE) {
        level_wave_update();
    }
    
    if (game.level_state == LEVEL_BOSS ||
        game.level_state == LEVEL_WAVE) {

        color_pad_update(&game.color_pad[0]);
        color_pad_update(&game.color_pad[1]);

        ctrl_input_mapping(gctx);
        ship_update(&game.p1);
        space_object_wrap_around(&game.space, &game.p1.cord.pos);

        int bullets_total = sizeof(game.p1.bullets)/sizeof(struct bullet);
        for (int i = 0; i < bullets_total; i++) {
            bullet_update(&game.p1.bullets[i]);
        }
        int bullets_total2 = sizeof(game.boss.bullets)/sizeof(struct bullet);
        for (int i = 0; i < bullets_total2; i++) {
            bullet_update(&game.boss.bullets[i]);
        }
        asteroid_update(&game.asteroids_pack, &game.space);

        if (collision_ship_color_pad_solve(&game.p1, &game.color_pad[0])) {
            ship_color(&game.p1, ASCOLOR_A);
            game_sfx_play(gctx, SFX_SHIP_POWERUP);
        } else if (collision_ship_color_pad_solve(&game.p1, &game.color_pad[1])) {
            ship_color(&game.p1, ASCOLOR_B);
            game_sfx_play(gctx, SFX_SHIP_POWERUP);
        }

        for (int a = 0; a < game.asteroids_pack.size; a++) {
            struct asteroid *asteroid = &game.asteroids_pack.pool[a];
            if (!asteroid->live) {
                continue;
            }
            if (collision_ship_asteroid_solve(&game.p1, asteroid) &&
                (GetTimeUnpaused() -game.p1.spawn_timestamp) > 2.0) {
                DEBUG_PRINT("ship collision with asteroid #%i\n", a);
                ship_destroy(gctx, &game.p1);
                ship_reset(&game.p1);
            }
            for (int b = 0; b < bullets_total; b++) {
                struct bullet *bullet = &game.p1.bullets[b];
                if (!bullet->live) {
                    continue;
                }
                if (collision_bullet_asteroid_solve(bullet, asteroid)) {
                    DEBUG_PRINT("bullet #%i collision with asteroid #%i\n", b, a);
                    DEBUG_PRINT("bullet %i gone\n", b);

                    if (asteroid->ascolor == ASCOLOR_N || bullet->ascolor == asteroid->ascolor) {
                        // full hit
                        DEBUG_PRINT("FUULLL HIT");
                        asteroid->hitpoints -= HIT_FULL;
                        game_sfx_play(gctx, SFX_ASTEROID_HIT_PLUS);
                    } else {
                        asteroid->hitpoints -= 0.1f;
                        game_sfx_play(gctx, SFX_ASTEROID_HIT);
                    }
                    bullet_destroy(bullet);

                    if (asteroid->hitpoints > 0.0) {
                        continue;
                    }
                    if (asteroid->type == ASTEROID_BIG) {
                        asteroid->type = ASTEROID_MED;
                        asteroid->cord.angle += (PI/4);
                        asteroid->spawn_time = 0.0;
                        asteroid_spawn(&game.asteroids_pack, asteroid);
                        asteroid->cord.angle -= (PI/2);
                        asteroid_spawn(&game.asteroids_pack, asteroid);
                    } else if (asteroid->type == ASTEROID_MED) {
                        asteroid->type = ASTEROID_SMALL;
                        asteroid->cord.angle += (PI/4);
                        asteroid->spawn_time = 0.0;
                        asteroid_spawn(&game.asteroids_pack, asteroid);
                        asteroid->cord.angle -= (PI/2);
                        asteroid_spawn(&game.asteroids_pack, asteroid);
                    }
                    asteroid_destroy(asteroid);
                    game_sfx_play(gctx, SFX_ASTEROID_EXPLOSION);
                    game.p1.ashit_timestamp = GetTimeUnpaused();
                    game.p1.ashit_combo++;
                    ship_apply_bonus(&game.p1);
                    if (game.p1.ashit_combo > game.p1.ashit_combo_max) {
                        game.p1.ashit_combo_max = game.p1.ashit_combo;
                    }
                }
            }
        }
    }
}

static void color_pad_draw(struct color_pad *color_pad) {
    if (!color_pad->show) {
        return;
    }

    Color color = color_get(color_pad->ascolor);
    DrawPoly(color_pad->cord.pos, 5, color_pad->size, color_pad->angle, color);
    float sizef = color_pad->size +(8.0*(1.0 + sinf(color_pad->angle/20))/2);
    DrawPolyLinesEx(color_pad->cord.pos, 5, sizef, color_pad->angle, 3.0, color);
}

static void game_draw(struct game_context *gctx) {
    BeginTextureMode(screen_texture);

#if 0
    Camera2D cam = { 0 };
    cam.target = Vector2Zero();
    cam.offset = Vector2Zero();
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;

    BeginMode2D(cam);
#endif

    ClearBackground(COLOR_BACKGROUND);

    DrawRectangleLines(game.space.left, game.space.top, game.space.width, game.space.height, COLOR_NEUTRAL);

    if (game.level_state == LEVEL_START) {
    } else if (game.level_state == LEVEL_START_1) {
        const struct level *level = &game.levels[game.level];
        DrawTextCentered(TextFormat("%s START", level->name), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -50, 50, COLOR_NEUTRAL);
        DrawTextCentered("Scoring", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +30, 40, COLOR_NEUTRAL);
        DrawTextCentered(">3 lives  = +1 Star", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +100, 30, COLOR_NEUTRAL);
        DrawTextCentered(TextFormat("Max combo > %i = +1 Star", level->max_combo), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +150, 30, COLOR_NEUTRAL);
        DrawTextCentered(TextFormat("Finish before %is = +1 Star", level->max_time), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +200, 30, COLOR_NEUTRAL);
    } else if (game.level_state == LEVEL_GAME_OVER) {
        const struct level *level = &game.levels[game.level];
        DrawRectangle(0, 0, SCREEN_WIDTH(), SCREEN_HEIGHT(), Fade(BLACK, 0.5f));
        DrawRectangle(SCREEN_WIDTH()/2 -200, SCREEN_HEIGHT()/2 -200, 400, 400, RAYWHITE);
        if (game.p1.lives <= 0) {
            DrawTextCentered(TextFormat("%s FAILED", level->name), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -180, 35, RED);
        } else {
            DrawTextCentered(TextFormat("%s CLEARED", level->name), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -180, 35, RED);
        }
        DrawTextCentered(TextFormat("Lives left: %i", game.p1.lives), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -120, 20, BLACK);
        DrawTextCentered(TextFormat("Level time: %.2f", game.level_time), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -80, 20, BLACK);
        DrawTextCentered(TextFormat("Max asteroid combo: %i", game.p1.ashit_combo_max), SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -40, 20, BLACK);
        DrawTextCentered("Submit Score", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2, 30, BLACK);
        DrawTextCentered("Retry", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +50, 30, BLACK);
        DrawTextCentered("Quit", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +100, 30, BLACK);
        DrawTextCentered(">", SCREEN_WIDTH()/2 -120, SCREEN_HEIGHT()/2 +(50*game.paused_sel), 30, BLACK);
    }
    if (game.level_state == LEVEL_WAVE ||
        game.level_state == LEVEL_BOSS) {
        asteroid_draw(&game.asteroids_pack);

        color_pad_draw(&game.color_pad[0]);
        color_pad_draw(&game.color_pad[1]);
        int bullets_total = sizeof(game.p1.bullets)/sizeof(struct bullet);
        int bullets_total2 = sizeof(game.boss.bullets)/sizeof(struct bullet);
        bullets_draw(game.p1.bullets, bullets_total);
        bullets_draw(game.boss.bullets, bullets_total2);
        ship_draw(&game.p1);

        DrawText(TextFormat("LIVES %i", game.p1.lives), 10, 30, 30, BLACK);
        DrawText(TextFormat("COMBO %i", game.p1.ashit_combo), 10, 60, 30, BLACK);

#if 0
    EndMode2D();
#endif
    }
    if (game.level_state == LEVEL_BOSS) {
        boss_draw(&game.boss);
    }

    if (game.paused) {
        DrawRectangle(0, 0, SCREEN_WIDTH(), SCREEN_HEIGHT(), Fade(BLACK, 0.5f));
        DrawRectangle(SCREEN_WIDTH()/2 -200, SCREEN_HEIGHT()/2 -200, 400, 400, RAYWHITE);
        DrawTextCentered("PAUSED", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 -180, 60, RED);
        DrawTextCentered("Continue", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2, 30, BLACK);
        DrawTextCentered("Retry", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +50, 30, BLACK);
        DrawTextCentered("Quit", SCREEN_WIDTH()/2, SCREEN_HEIGHT()/2 +100, 30, BLACK);
        DrawTextCentered(">", SCREEN_WIDTH()/2 -100, SCREEN_HEIGHT()/2 +(50*game.paused_sel), 30, BLACK);
    }
    EndTextureMode();

    BeginDrawing();

    ClearBackground(COLOR_BACKGROUND);

    Rectangle splitScreenRect = { 0.0f, 0.0f, (float)screen_texture.texture.width, (float)-screen_texture.texture.height };

    Vector2 texture_offset = { (GetScreenWidth() - SCREEN_WIDTH())/2.0f, (GetScreenHeight() - SCREEN_HEIGHT())/2.0f };
    DrawTextureRec(screen_texture.texture, splitScreenRect, texture_offset, WHITE);

    DrawText(TextFormat("%ix%i@%i", GetScreenWidth(), GetScreenHeight(), GetFPS()), 30, 30, 30, GREEN);
    //DrawText(TextFormat("%.3f", GetTimeUnpaused()), 30, 70, 30, GREEN);
    //DrawText(TextFormat("%.3f", GetTimeUnpaused() -game.level_timestamp), 30, 110, 30, GREEN);

    if (GetScreenWidth() < SCREEN_WIDTH() || GetScreenHeight() < SCREEN_HEIGHT()) {
        const char *screen_fail_txt = "SCREEN TOO SMALL";
        int screen_fail_width = MeasureText(screen_fail_txt, 50);
        Rectangle screen_fail_rect = { 0, 0, GetScreenWidth(), GetScreenHeight() };
        DrawRectangleLinesEx(screen_fail_rect, 5.0f, RED);
        DrawText(screen_fail_txt, (GetScreenWidth() -screen_fail_width)/2, GetScreenHeight()/2, 50, RED);
    }

    EndDrawing();
}

struct game_state _gs_game = {
    .init = game_init,
    .deinit = NULL,
    .enter = game_enter,
    .exit = game_exit,
    .update = game_update,
    .draw = game_draw
};
