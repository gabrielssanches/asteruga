#ifndef GA_SHIP_H
#define GA_SHIP_H

enum ship_state {
    SHIP_INIT,
    SHIP_LIVE,
};

struct ship {
    struct position2d cord;
    enum ship_state state;

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

void _ship_init(struct ship *ship, Vector2 *pos);
void _ship_spawn(struct ship *ship);
void _ship_update(struct ship *ship);
void _ship_draw(struct ship *ship);

#endif // GA_SHIP_H
