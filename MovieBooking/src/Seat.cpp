#include "../include/Seat.h"

Seat::Seat() {}

Seat::Seat(int i, Rectangle r) {
    id = i;
    rect = r;
    state = AVAILABLE;
}

bool Seat::IsClicked(Vector2 mouse) {
    return CheckCollisionPointRec(mouse, rect);
}

void Seat::Draw() {
    Color c;

    if (state == AVAILABLE) c = GREEN;
    else if (state == SELECTED) c = YELLOW;
    else c = RED;

    DrawRectangleRec(rect, c);
    DrawRectangleLinesEx(rect, 2, BLACK);
}