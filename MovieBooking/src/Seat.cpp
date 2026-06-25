#include "../include/Seat.h"
#include "../include/Theme.h"
#include <algorithm>

Seat::Seat() {}

Seat::Seat(int i, Rectangle r) {
    id = i;
    rect = r;
    state = AVAILABLE;
}

bool Seat::IsClicked(Vector2 mouse) {
    float cx = rect.x + rect.width / 2.0f;
    float cy = rect.y + rect.height / 2.0f;
    float r = std::min(rect.width, rect.height) / 2.0f;
    float dx = mouse.x - cx;
    float dy = mouse.y - cy;
    return (dx*dx + dy*dy) <= (r * r);
}

void Seat::Draw() {
    Color c;

    if (state == AVAILABLE) c = Theme::SeatAvailable();
    else if (state == SELECTED) c = Theme::SeatSelected();
    else c = Theme::SeatBooked();

    float cx = rect.x + rect.width / 2.0f;
    float cy = rect.y + rect.height / 2.0f;
    float r = std::min(rect.width, rect.height) / 2.0f;

    DrawCircleV(Vector2{cx, cy}, r, c);
    DrawCircleLines((int)cx, (int)cy, r, Theme::Outline());
}
