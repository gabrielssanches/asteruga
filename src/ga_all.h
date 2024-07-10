#ifndef GA_ALL_H
#define GA_ALL_H

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

#endif // GA_ALL_H
