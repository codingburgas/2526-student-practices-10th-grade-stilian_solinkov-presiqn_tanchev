#include "../include/Theme.h"

namespace Theme {
    Mode mode = LIGHT;

    void Toggle() {
        mode = (mode == LIGHT) ? DARK : LIGHT;
    }

    Color Background() { return (mode == DARK) ? Color{ 11, 18, 32, 255 } : RAYWHITE; }
    Color Text() { return (mode == DARK) ? Color{ 230, 238, 248, 255 } : BLACK; }
    Color SecondaryText() { return (mode == DARK) ? Color{ 150, 160, 180, 255 } : GRAY; }
    Color Primary() { return (mode == DARK) ? Color{ 96, 165, 250, 255 } : BLUE; }
    Color Button() { return (mode == DARK) ? Color{ 10, 25, 60, 255 } : GRAY; }
    Color ButtonHover() { return (mode == DARK) ? Color{ 30, 60, 120, 255 } : DARKBLUE; }
    Color ButtonText() { return (mode == DARK) ? Color{ 230, 238, 248, 255 } : BLACK; }
    Color Panel() { return (mode == DARK) ? Color{ 7, 16, 36, 255 } : LIGHTGRAY; }
    Color Outline() { return (mode == DARK) ? Color{ 200, 200, 200, 255 } : BLACK; }
    Color Success() { return (mode == DARK) ? Color{ 50, 200, 100, 255 } : GREEN; }
    Color Progress() { return (mode == DARK) ? Color{ 70, 160, 240, 255 } : GREEN; }

    Color SeatAvailable() { return (mode == DARK) ? Color{ 30, 80, 160, 255 } : DARKBLUE; }
    Color SeatSelected() { return (mode == DARK) ? Color{ 220, 180, 60, 255 } : YELLOW; }
    Color SeatBooked() { return (mode == DARK) ? Color{ 200, 70, 70, 255 } : RED; }
}