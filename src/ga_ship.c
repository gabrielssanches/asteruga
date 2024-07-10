#include "asteruga.h"

void _ship_init(struct ship *ship, Vector2 *pos) {
    ship->state = SHIP_INIT;

    ship->cord.pos = (pos != NULL) ? *pos : Vector2Zero();
    ship->cord.speed = (Vector2) { 1, 1 };//Vector2Zero();
    ship->cord.angle = -PI/2.0f; // facing upwards
    ship->size = 25;
}

void _ship_spawn(struct ship *ship) {
    ship->state = SHIP_LIVE;
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

void _ship_update(struct ship *ship) {
    if (ship->state == SHIP_INIT) {
        return;
    }

    ship->cord.pos = Vector2Add(ship->cord.pos, ship->cord.speed); 

    ship->cord.speed.x = deaccel(ship->cord.speed.x);
    ship->cord.speed.y = deaccel(ship->cord.speed.y);
}

void _ship_draw(struct ship *ship) {
    if (ship->state == SHIP_INIT) {
        return;
    }


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

    DrawTriangle(s0r, s1r, s2r, COLOR_NEUTRAL);
}
