#include "asteruga.h"

void DrawTextCentered(const char *text, int pos_x, int pos_y, int fontSize, Color color) {
    int text_width = MeasureText(text, fontSize); 
    DrawText(text, pos_x -(text_width/2), pos_y, fontSize, color);
}
