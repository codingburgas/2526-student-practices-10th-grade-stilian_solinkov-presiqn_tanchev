#include "../include/Seat.h"
#include "../include/Theme.h"

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

    if (state == AVAILABLE) c = Theme::SeatAvailable();
    else if (state == SELECTED) c = Theme::SeatSelected();
    else c = Theme::SeatBooked();

    DrawRectangleRec(rect, c);
    DrawRectangleLinesEx(rect, 2, Theme::Outline());
}