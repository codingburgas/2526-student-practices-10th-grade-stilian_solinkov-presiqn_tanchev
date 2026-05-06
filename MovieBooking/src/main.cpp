#include "raylib.h"
#include "../include/Movie.h"
#include "../include/Show.h"
#include <vector>

enum AppState {
    MAIN_MENU,
    BOOKING,
    PAYMENT,
    CONFIRMATION
};

AppState state = MAIN_MENU;

int selectedMovie = -1;
Show* currentShow = nullptr;

int finalPrice = 0;
float paymentTimer = 0;

std::vector<Movie> movies;

void InitMovies() {

    movies.push_back(Movie("Interstellar", "EN", "Sci-Fi", 12, "assets/images/interstellar.png"));
    movies.push_back(Movie("Avatar 2", "EN", "Action", 15, "assets/images/avatar.png"));
    movies.push_back(Movie("Oppenheimer", "EN", "Drama", 18, "assets/images/oppenheimer.png"));
    movies.push_back(Movie("Batman", "EN", "Action", 10, "assets/images/batman.png"));
    movies.push_back(Movie("Inception", "EN", "Sci-Fi", 14, "assets/images/inception.png"));
    movies.push_back(Movie("Joker", "EN", "Drama", 11, "assets/images/joker.png"));
    movies.push_back(Movie("Titanic", "EN", "Romance", 9, "assets/images/titanic.png"));
    movies.push_back(Movie("Dune", "EN", "Sci-Fi", 16, "assets/images/dune.png"));
    movies.push_back(Movie("Matrix", "EN", "Action", 13, "assets/images/matrix.png"));
}

int main() {

    InitWindow(1000, 700, "SiCinema");
    SetTargetFPS(60);

    InitMovies();

    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ================= MAIN MENU =================
        if (state == MAIN_MENU) {

            DrawText("SiCinema", 420, 10, 40, DARKBLUE);

            int cols = 3;
            int w = 280;
            int h = 140;

            for (int i = 0; i < movies.size(); i++) {

                int x = 80 + (i % cols) * (w + 20);
                int y = 120 + (i / cols) * (h + 20);

                Rectangle card = { (float)x, (float)y, (float)w, (float)h };

                bool hover = CheckCollisionPointRec(mouse, card);

                DrawRectangleRec(card, hover ? LIGHTGRAY : GRAY);
                DrawRectangleLinesEx(card, 2, BLACK);

                DrawTexturePro(
                    movies[i].poster,
                    { 0,0,(float)movies[i].poster.width,(float)movies[i].poster.height },
                    { (float)x + 10,(float)y + 10,80,100 },
                    { 0,0 },
                    0,
                    WHITE
                );

                DrawText(movies[i].title.c_str(), x + 100, y + 10, 20, BLACK);
                DrawText(TextFormat("Price: %d$", movies[i].price), x + 100, y + 50, 20, DARKGREEN);

                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedMovie = i;
                    currentShow = new Show(&movies[i]);
                    currentShow->InitSeats();
                    state = BOOKING;
                }
            }
        }

        // ================= BOOKING =================
        else if (state == BOOKING) {

            DrawText(movies[selectedMovie].title.c_str(), 20, 20, 30, DARKBLUE);
            DrawText(TextFormat("Price: %d$", movies[selectedMovie].price), 20, 60, 25, DARKGREEN);
            DrawText(TextFormat("Total: %d$", currentShow->GetTotalPrice()), 20, 100, 25, MAROON);

            Rectangle payBtn = { 20,160,200,50 };
            Rectangle backBtn = { 240,160,200,50 };

            DrawRectangleRec(payBtn, DARKGREEN);
            DrawText("PAY", 90, 175, 20, WHITE);

            DrawRectangleRec(backBtn, GRAY);
            DrawText("BACK", 300, 175, 20, BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, payBtn)) {
                    finalPrice = currentShow->GetTotalPrice();
                    if (finalPrice > 0) {
                        state = PAYMENT;
                        paymentTimer = 0;
                    }
                }

                if (CheckCollisionPointRec(mouse, backBtn)) {
                    delete currentShow;
                    currentShow = nullptr;
                    state = MAIN_MENU;
                }
            }

            currentShow->Update();
            currentShow->Draw();
        }

        // ================= PAYMENT =================
        else if (state == PAYMENT) {

            DrawText("PROCESSING PAYMENT...", 300, 200, 30, DARKBLUE);
            DrawText(TextFormat("Amount: %d$", finalPrice), 350, 260, 25, BLACK);

            DrawRectangle(300, 320, 400, 40, GRAY);

            paymentTimer += GetFrameTime() * 50;
            if (paymentTimer > 100) paymentTimer = 100;

            DrawRectangle(300, 320, (int)paymentTimer * 4, 40, GREEN);

            if (paymentTimer >= 100)
                state = CONFIRMATION;
        }

        // ================= CONFIRMATION =================
        else if (state == CONFIRMATION) {

            DrawText("PAYMENT SUCCESSFUL!", 280, 200, 40, GREEN);
            DrawText(TextFormat("Paid: %d$", finalPrice), 360, 270, 25, DARKBLUE);

            Rectangle backBtn = { 350,400,250,60 };

            DrawRectangleRec(backBtn, DARKBLUE);
            DrawText("BACK TO MENU", 390, 420, 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, backBtn)) {

                    currentShow->ConfirmBooking();
                    currentShow->SaveBookedSeats();

                    delete currentShow;
                    currentShow = nullptr;

                    state = MAIN_MENU;
                }
            }
        }

        EndDrawing();
    }

    for (auto& m : movies)
        UnloadTexture(m.poster);

    CloseWindow();
    return 0;
}