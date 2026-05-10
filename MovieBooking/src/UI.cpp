#include "../include/UI.h"
#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"

void UI::Init(Show* s) {
    show = s;
}

void UI::Update() {
    show->Update();
}

void UI::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("MOVIE BOOKING SYSTEM", 20, 20, 20, DARKBLUE);
    DrawText("Click seats | ENTER = book", 20, 50, 15, DARKGRAY);

    show->Draw();

    EndDrawing();
}