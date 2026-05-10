#pragma once
#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"

enum SeatState {
    AVAILABLE,
    SELECTED,
    BOOKED
};

class Seat {
public:
    int id;
    Rectangle rect;
    SeatState state;

    Seat();
    Seat(int i, Rectangle r);

    void Draw();
    bool IsClicked(Vector2 mouse);
};