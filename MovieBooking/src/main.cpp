#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"
#include "../include/Movie.h"
#include "../include/Show.h"
#include <fstream>
#include <vector>

enum AppState {
    HOME,
    MAIN_MENU,
    BOOKING,
    PAYMENT,
    CONFIRMATION,
    RECENT_BOOKINGS
};

AppState state = HOME;

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

        if (state == HOME) {

            // Background
            ClearBackground(RAYWHITE);

            // Title
            DrawText("SiCinema", 340, 120, 60, DARKBLUE);

            DrawText("Movie Ticket Booking System", 300, 190, 25, LIGHTGRAY);

            // Buttons
            Rectangle menuBtn = { 350, 300, 300, 60 };
            Rectangle recentBtn = { 350, 390, 300, 60 };
            Rectangle exitBtn = { 350, 480, 300, 60 };

            Vector2 mouse = GetMousePosition();

            // Menu button
            bool menuHover = CheckCollisionPointRec(mouse, menuBtn);
            DrawRectangleRec(menuBtn, menuHover ? DARKBLUE : BLUE);
            DrawText("MENU", 455, 318, 25, WHITE);

            // Recent bookings button
            bool recentHover = CheckCollisionPointRec(mouse, recentBtn);
            DrawRectangleRec(recentBtn, recentHover ? DARKBLUE : BLUE);
            DrawText("RECENT BOOKINGS", 390, 408, 25, WHITE);

            // Exit button
            bool exitHover = CheckCollisionPointRec(mouse, exitBtn);
            DrawRectangleRec(exitBtn, exitHover ? DARKBLUE : BLUE);
            DrawText("EXIT", 465, 498, 25, WHITE);

            // Mouse Input
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, menuBtn)) {
                    state = MAIN_MENU;
                }

                if (CheckCollisionPointRec(mouse, recentBtn)) {
                    state = RECENT_BOOKINGS;
                }

                if (CheckCollisionPointRec(mouse, exitBtn)) {
                    CloseWindow();
                    return 0;
                }
            }
        }

        else if (state == RECENT_BOOKINGS) {

            DrawText("Recent Bookings", 320, 40, 40, DARKBLUE);

            std::ifstream file("data/bookings.txt");

            std::string line;
            int y = 140;

            while (getline(file, line)) {
                DrawText(line.c_str(), 80, y, 22, BLACK);
                y += 40;
            }

            file.close();

            Rectangle backBtn = { 350, 600, 250, 60 };

            DrawRectangleRec(backBtn, GRAY);
            DrawText("BACK", 450, 620, 25, BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(GetMousePosition(), backBtn)) {
                    state = HOME;
                }
            }
        }

        else if (state == MAIN_MENU) {

            DrawText("SiCinema", 420, 10, 40, DARKBLUE);

            Rectangle backBtn = { 350, 600, 250, 60 };

            DrawRectangleRec(backBtn, GRAY);
            DrawText("BACK", 450, 620, 25, BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(GetMousePosition(), backBtn)) {
                    state = HOME;
                }
            }

            int cols = 3;
            int w = 280;
            int h = 140;

            for (int i = 0; i < movies.size(); i++) {

                int x = 80 + (i % cols) * (w + 20);
                int y = 120 + (i / cols) * (h + 20);

                Rectangle card = { (float)x, (float)y, (float)w, (float)h };

                bool hover = CheckCollisionPointRec(mouse, card);

                DrawRectangleRec(card, hover ? DARKBLUE : BLUE);
                DrawRectangleLinesEx(card, 2, BLACK);

                DrawTexturePro(
                    movies[i].poster,
                    { 0,0,(float)movies[i].poster.width,(float)movies[i].poster.height },
                    { (float)x + 10,(float)y + 10,80,100 },
                    { 0,0 },
                    0,
                    WHITE
                );

                DrawText(movies[i].title.c_str(), x + 100, y + 10, 20, WHITE);
                DrawText(TextFormat("Price: %d$", movies[i].price), x + 100, y + 50, 20, WHITE);

                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedMovie = i;
                    currentShow = new Show(&movies[i]);
                    currentShow->InitSeats();
                    state = BOOKING;
                }
            }
        }

        else if (state == BOOKING) {

            DrawText(movies[selectedMovie].title.c_str(), 20, 20, 30, DARKBLUE);
            DrawText(TextFormat("Price: %d$", movies[selectedMovie].price), 20, 60, 25, DARKGREEN);
            DrawText(TextFormat("Total: %d$", currentShow->GetTotalPrice()), 20, 100, 25, MAROON);

            Rectangle payBtn = { 20,160,200,50 };
            Rectangle backBtn = { 240,160,200,50 };

            DrawRectangleRec(payBtn, DARKBLUE);
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