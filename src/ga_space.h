#ifndef GA_SPACE_H
#define GA_SPACE_H

struct space {
    float top;
    float bot;
    float left;
    float right;
    float width;
    float height;
    Rectangle limits;
};

void space_init(struct space *space, Rectangle limits);

#endif // GA_SPACE_H
