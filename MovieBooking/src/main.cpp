#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"
#include "../include/Movie.h"
#include "../include/Show.h"
#include "../include/Theme.h"
#include "../include/Booking.h"
#include <fstream>
#include <string>
#include <vector>

void SaveBooking(const std::string& movieTitle, int price) {
    std::ofstream file("assets/bookings.txt", std::ios::app);

    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "FAILED TO OPEN bookings.txt");
        return;
    }

    file << "Movie: " << movieTitle
        << " | Paid: " << price << "$\n";

    file.close();
}

enum AppState {
    LOGIN,
    HOME,
    MAIN_MENU,
    CART,
    BOOKING,
    PAYMENT,
    CONFIRMATION,
    ACCOUNT
};

AppState state = HOME;
bool loggedIn = false;
std::vector<Booking> cart;
std::string cartMessage;

int selectedMovie = -1;
Show* currentShow = nullptr;

int finalPrice = 0;
float paymentTimer = 0;

std::vector<Movie> movies;

// Login UI state
std::string loginUsername;
std::string loginPassword;
std::string loginMessage;
bool typingUsername = false;
bool typingPassword = false;

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
        ClearBackground(Theme::Background());

        if (state == LOGIN) {

            // Draw login screen
            DrawText("Login", 420, 120, 48, Theme::Primary());

            Rectangle userRect = { 300, 220, 400, 40 };
            Rectangle passRect = { 300, 280, 400, 40 };
            Rectangle loginBtn = { 430, 340, 140, 40 };

            bool userHover = CheckCollisionPointRec(mouse, userRect);
            bool passHover = CheckCollisionPointRec(mouse, passRect);
            bool loginHover = CheckCollisionPointRec(mouse, loginBtn);

            DrawRectangleRec(userRect, typingUsername ? Theme::ButtonHover() : Theme::Panel());
            DrawRectangleLinesEx(userRect, 2, Theme::Outline());
            DrawText("Username:", 310, 226, 18, Theme::Text());
            DrawText(loginUsername.c_str(), 420, 226, 18, Theme::Text());

            DrawRectangleRec(passRect, typingPassword ? Theme::ButtonHover() : Theme::Panel());
            DrawRectangleLinesEx(passRect, 2, Theme::Outline());
            DrawText("Password:", 310, 286, 18, Theme::Text());
            std::string masked(loginPassword.size(), '*');
            DrawText(masked.c_str(), 420, 286, 18, Theme::Text());

            DrawRectangleRec(loginBtn, loginHover ? Theme::ButtonHover() : Theme::Primary());
            DrawText("LOGIN", 475, 351, 20, Theme::ButtonText());

            if (!loginMessage.empty()) {
                DrawText(loginMessage.c_str(), 360, 400, 18, Theme::Text());
            }

            // Input handling
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, userRect)) {
                    typingUsername = true; typingPassword = false;
                } else if (CheckCollisionPointRec(mouse, passRect)) {
                    typingPassword = true; typingUsername = false;
                } else if (CheckCollisionPointRec(mouse, loginBtn)) {
                    // attempt login
                    if (loginUsername.empty() || loginPassword.empty()) {
                        loginMessage = "Please enter username and password";
                    } else {
                    loginMessage = "Login successful";
                        typingUsername = typingPassword = false;
                        state = HOME;
                        loggedIn = true;
                    }
                } else {
                    typingUsername = typingPassword = false;
                }
            }

            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125) {
                    if (typingUsername && loginUsername.size() < 64) loginUsername.push_back((char)key);
                    else if (typingPassword && loginPassword.size() < 64) loginPassword.push_back((char)key);
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (typingUsername && !loginUsername.empty()) loginUsername.pop_back();
                if (typingPassword && !loginPassword.empty()) loginPassword.pop_back();
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (loginUsername.empty() || loginPassword.empty()) {
                    loginMessage = "Please enter username and password";
                } else {
                    loginMessage = "Login successful";
                    typingUsername = typingPassword = false;
                    state = HOME;
                    loggedIn = true;
                }
            }

        } else if (state == HOME) {

            // Title
            DrawText("SiCinema", 340, 120, 60, Theme::Primary());

            DrawText("Movie Ticket Booking System", 300, 190, 25, Theme::SecondaryText());

            // Buttons
            Rectangle menuBtn = { 350, 300, 300, 60 };
            Rectangle recentBtn = { 350, 390, 300, 60 };
            Rectangle exitBtn = { 350, 480, 300, 60 };
            // Theme button on main page
            Rectangle themeBtn = { 820, 20, 160, 40 };

            // Menu button
            bool menuHover = CheckCollisionPointRec(mouse, menuBtn);
            DrawRectangleRec(menuBtn, menuHover ? Theme::ButtonHover() : Theme::Button());
            DrawText("MENU", 455, 318, 25, Theme::ButtonText());

            // Cart button (always accessible)
            bool recentHover = CheckCollisionPointRec(mouse, recentBtn);
            DrawRectangleRec(recentBtn, recentHover ? Theme::ButtonHover() : Theme::Button());
            DrawText("CART", 435, 408, 25, Theme::ButtonText());

            // Login / Account / Logout button next to theme button
            Rectangle loginBtn = { 650, 20, 160, 40 };
            bool loginHover = CheckCollisionPointRec(mouse, loginBtn);
            DrawRectangleRec(loginBtn, loginHover ? Theme::ButtonHover() : Theme::Button());
            const char* topLabel = (!loggedIn) ? "Login" : (state == ACCOUNT ? "Logout" : "Account");
            DrawText(topLabel, 688, 30, 18, Theme::ButtonText());

            // Theme button
            bool themeHover = CheckCollisionPointRec(mouse, themeBtn);
            DrawRectangleRec(themeBtn, themeHover ? Theme::ButtonHover() : Theme::Button());
            DrawText("Theme", 838, 30, 18, Theme::ButtonText());

            // Exit button
            bool exitHover = CheckCollisionPointRec(mouse, exitBtn);
            DrawRectangleRec(exitBtn, exitHover ? Theme::ButtonHover() : Theme::Button());
            DrawText("EXIT", 465, 498, 25, Theme::ButtonText());

            // Mouse Input
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, menuBtn)) {
                    state = MAIN_MENU;
                }

                if (CheckCollisionPointRec(mouse, recentBtn)) {
                    state = CART;
                }

                if (CheckCollisionPointRec(mouse, loginBtn)) {
                    if (!loggedIn) state = LOGIN;
                    else {
                        if (state == ACCOUNT) {
                            // perform logout
                            loggedIn = false;
                            loginUsername.clear();
                            state = HOME;
                        } else {
                            state = ACCOUNT;
                        }
                    }
                }

                if (CheckCollisionPointRec(mouse, themeBtn)) {
                    Theme::Toggle();
                }

                if (CheckCollisionPointRec(mouse, exitBtn)) {
                    CloseWindow();
                    return 0;
                }
            }
        }

        else if (state == ACCOUNT)
        {
            // Greeting
            std::string greet = "Hello " + loginUsername;
            DrawText(greet.c_str(), 80, 30, 40, Theme::Primary());

            // Recent bookings list below greeting
            std::ifstream file("assets/bookings.txt");

            int y = 100;

            std::string bookingId;
            std::string movie;
            std::string seats;
            std::string total;
            std::string separator;

            while (
                getline(file, bookingId) &&
                getline(file, movie) &&
                getline(file, seats) &&
                getline(file, total) &&
                getline(file, separator))
            {
                Rectangle card = { 80, (float)y, 800, 120 };

                DrawRectangleRounded(card, 0.15f, 10, LIGHTGRAY);
                DrawRectangleRoundedLines(card, 0.15f, 10, DARKGRAY);

                DrawText(movie.c_str(), 110, y + 15, 24, DARKBLUE);

                DrawText(bookingId.c_str(), 110, y + 50, 18, BLACK);

                DrawText(seats.c_str(), 110, y + 75, 18, BLACK);

                DrawText(total.c_str(), 550, y + 50, 20, DARKGREEN);

                y += 140;
            }

            file.close();

            // Logout button (visible when logged in)
            Rectangle logoutBtn = { 820, 20, 160, 40 };
            bool logoutHover = CheckCollisionPointRec(mouse, logoutBtn);
            if (loggedIn) {
                DrawRectangleRec(logoutBtn, logoutHover ? Theme::ButtonHover() : Theme::Button());
                DrawText("Logout", 860, 30, 18, Theme::ButtonText());
            }

            Rectangle backBtn = { 400, 600, 200, 50 };

            DrawRectangleRec(backBtn, GRAY);
            DrawText("BACK", 470, 615, 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(GetMousePosition(), backBtn))
                {
                    state = HOME;
                }

                if (loggedIn && CheckCollisionPointRec(GetMousePosition(), logoutBtn)) {
                    loggedIn = false;
                    loginUsername.clear();
                    state = HOME;
                }
            }
        }

        else if (state == MAIN_MENU) {

            DrawText("SiCinema", 420, 10, 40, Theme::Primary());

            Rectangle backBtn = { 350, 600, 250, 60 };

            DrawRectangleRec(backBtn, Theme::Button());
            DrawText("BACK", 450, 620, 25, Theme::ButtonText());

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

                DrawRectangleRec(card, hover ? Theme::ButtonHover() : Theme::Button());
                DrawRectangleLinesEx(card, 2, Theme::Outline());

                DrawTexturePro(
                    movies[i].poster,
                    { 0,0,(float)movies[i].poster.width,(float)movies[i].poster.height },
                    { (float)x + 10,(float)y + 10,80,100 },
                    { 0,0 },
                    0,
                    WHITE
                );

                DrawText(movies[i].title.c_str(), x + 100, y + 10, 20, Theme::ButtonText());
                DrawText(TextFormat("Price: %d$", movies[i].price), x + 100, y + 50, 20, Theme::ButtonText());

                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedMovie = i;
                    currentShow = new Show(&movies[i]);
                    currentShow->InitSeats();

                    currentShow->LoadBookedSeats();

                    state = BOOKING;
                }
            }
        }

        else if (state == BOOKING) {

            DrawText(movies[selectedMovie].title.c_str(), 20, 20, 30, Theme::Primary());
            DrawText(TextFormat("Price: %d$", movies[selectedMovie].price), 20, 60, 25, Theme::Success());
            DrawText(TextFormat("Total: %d$", currentShow->GetTotalPrice()), 20, 100, 25, Theme::Text());

            Rectangle payBtn = { 20,160,200,50 };
            Rectangle backBtn = { 240,160,200,50 };

            DrawRectangleRec(payBtn, Theme::Primary());
            DrawText("ADD TO CART", 60, 175, 20, Theme::ButtonText());

            DrawRectangleRec(backBtn, Theme::Button());
            DrawText("BACK", 300, 175, 20, Theme::ButtonText());

            if (currentShow != nullptr) {
                currentShow->Update();
                currentShow->Draw();
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, payBtn)) {
                    // Add selected seats to cart (anonymous allowed)
                    Booking b;
                    b.bookingId = rand() % 10000;
                    b.movieTitle = movies[selectedMovie].title;
                    b.totalPrice = currentShow->GetTotalPrice();

                    for (auto& s : currentShow->seats) {
                        if (s.state == SELECTED) {
                            b.seatIds.push_back(s.id);
                            s.state = AVAILABLE; // release selection
                        }
                    }

                    if (!b.seatIds.empty()) {
                        cart.push_back(b);
                        cartMessage = "Added to cart";
                    } else {
                        cartMessage = "No seats selected";
                    }
                }

                if (CheckCollisionPointRec(mouse, backBtn)) {
                    delete currentShow;
                    currentShow = nullptr;

                    state = MAIN_MENU;
                }
            }

            DrawTexturePro(
                movies[selectedMovie].poster,
                { 0, 0,
                  (float)movies[selectedMovie].poster.width,
                  (float)movies[selectedMovie].poster.height },

                { 550, 60, 400, 600 },

                { 0, 0 },
                0,
                WHITE
            );

        }

        else if (state == PAYMENT) {

            DrawText("PROCESSING PAYMENT...", 300, 200, 30, Theme::Primary());
            DrawText(TextFormat("Amount: %d$", finalPrice), 350, 260, 25, Theme::Text());

            DrawRectangle(300, 320, 400, 40, Theme::Button());

            paymentTimer += GetFrameTime() * 50;
            if (paymentTimer > 100) paymentTimer = 100;

            DrawRectangle(300, 320, (int)paymentTimer * 4, 40, Theme::Progress());

            if (paymentTimer >= 100)
                state = CONFIRMATION;
        }

        else if (state == CONFIRMATION) {

            DrawText("PAYMENT SUCCESSFUL!", 280, 200, 40, Theme::Success());
            DrawText(TextFormat("Paid: %d$", finalPrice), 360, 270, 25, Theme::Primary());

            Rectangle backBtn = { 350,400,250,60 };

            DrawRectangleRec(backBtn, Theme::Primary());
            DrawText("BACK TO MENU", 390, 420, 20, Theme::ButtonText());

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, backBtn)) {

                    Booking booking;

                    booking.bookingId = rand() % 10000;
                    booking.movieTitle = movies[selectedMovie].title;
                    booking.totalPrice = finalPrice;

                    for (auto& s : currentShow->seats)
                    {
                        if (s.state == SELECTED)
                        {
                            booking.seatIds.push_back(s.id);
                        }
                    }

                    booking.SaveToFile();

                    currentShow->ConfirmBooking();
                    currentShow->SaveBookedSeats();

                    delete currentShow;
                    currentShow = nullptr;

                    state = MAIN_MENU;
                }
            }
        }

        // Theme toggle: press T to toggle theme
        if (IsKeyPressed(KEY_T)) Theme::Toggle();

        EndDrawing();
    }

    for (auto& m : movies)
        UnloadTexture(m.poster);

    CloseWindow();
    return 0;
}