#include "../include/UI.h"
#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"
#include "../include/Theme.h"

void UI::Init(Show* s) {
    show = s;
}

void UI::Update() {
    show->Update();
}

void UI::Draw() {
    BeginDrawing();
    ClearBackground(Theme::Background());

    DrawText("MOVIE BOOKING SYSTEM", 20, 20, 20, Theme::Primary());
    DrawText("Click seats | ENTER = book", 20, 50, 15, Theme::SecondaryText());

    show->Draw();

    EndDrawing();
}