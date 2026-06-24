#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"
#include "../include/Movie.h"
#include "../include/Show.h"
#include "../include/Theme.h"
#include "../include/Booking.h"
#include <algorithm>
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

// Email validation function
bool IsValidEmail(const std::string& email) {
    return email.find('@') != std::string::npos;
}

enum AppState {
    LOGIN,
    REGISTER,
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
std::string searchText;
bool typingSearch = false;

// Login UI state
std::string loginUsername;
std::string loginPassword;
std::string loginMessage;
bool typingUsername = false;
bool typingPassword = false;

// Register UI state
std::string registerUsername;
std::string registerEmail;
std::string registerPassword;
std::string registerConfirmPassword;
std::string registerMessage;
bool typingRegUsername = false;
bool typingRegEmail = false;
bool typingRegPassword = false;
bool typingRegConfirmPassword = false;

// Registered accounts (username:password:email format)
std::vector<std::string> accounts;

// Save accounts to file
void SaveAccountsToFile() {
    std::ofstream file("assets/accounts.txt");
    for (const auto& acc : accounts) {
        file << acc << "\n";
    }
    file.close();
}

// Load accounts from file
void LoadAccountsFromFile() {
    std::ifstream file("assets/accounts.txt");
    std::string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            accounts.push_back(line);
        }
    }
    file.close();
}

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
    LoadAccountsFromFile();  // Load saved accounts at startup

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

            // Create Account button
            Rectangle createBtn = { 430, 400, 140, 40 };
            bool createHover = CheckCollisionPointRec(mouse, createBtn);
            DrawRectangleRec(createBtn, createHover ? Theme::ButtonHover() : Theme::Button());
            DrawText("CREATE ACCOUNT", 420, 408, 16, Theme::ButtonText());

            if (!loginMessage.empty()) {
                DrawText(loginMessage.c_str(), 360, 450, 18, Theme::Text());
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
                } else if (CheckCollisionPointRec(mouse, createBtn)) {
                    // Go to register page
                    state = REGISTER;
                    loginMessage = "";
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

        } else if (state == REGISTER) {

            // Draw register screen
            DrawText("Create Account", 380, 80, 48, Theme::Primary());

            Rectangle userRect = { 200, 160, 600, 40 };
            Rectangle emailRect = { 200, 220, 600, 40 };
            Rectangle passRect = { 200, 280, 600, 40 };
            Rectangle confRect = { 200, 340, 600, 40 };
            Rectangle registerBtn = { 430, 400, 140, 40 };
            Rectangle backBtn = { 350, 450, 300, 40 };

            DrawRectangleRec(userRect, typingRegUsername ? Theme::ButtonHover() : Theme::Panel());
            DrawRectangleLinesEx(userRect, 2, Theme::Outline());
            DrawText("Username:", 220, 166, 18, Theme::Text());
            DrawText(registerUsername.c_str(), 400, 166, 18, Theme::Text());

            DrawRectangleRec(emailRect, typingRegEmail ? Theme::ButtonHover() : Theme::Panel());
            // Show email validation state: red border if invalid (when not typing), green if valid
            Color emailBorderColor = (!registerEmail.empty() && !IsValidEmail(registerEmail)) ? RED : Theme::Outline();
            DrawRectangleLinesEx(emailRect, 2, emailBorderColor);
            DrawText("Email:", 220, 226, 18, Theme::Text());
            DrawText(registerEmail.c_str(), 400, 226, 18, Theme::Text());

            // Show email validation warning
            if (!registerEmail.empty() && !IsValidEmail(registerEmail)) {
                DrawText("(missing @)", 400, 243, 14, RED);
            }

            DrawRectangleRec(passRect, typingRegPassword ? Theme::ButtonHover() : Theme::Panel());
            DrawRectangleLinesEx(passRect, 2, Theme::Outline());
            DrawText("Password:", 220, 286, 18, Theme::Text());
            std::string masked1(registerPassword.size(), '*');
            DrawText(masked1.c_str(), 400, 286, 18, Theme::Text());

            DrawRectangleRec(confRect, typingRegConfirmPassword ? Theme::ButtonHover() : Theme::Panel());
            DrawRectangleLinesEx(confRect, 2, Theme::Outline());
            DrawText("Confirm:", 220, 346, 18, Theme::Text());
            std::string masked2(registerConfirmPassword.size(), '*');
            DrawText(masked2.c_str(), 400, 346, 18, Theme::Text());

            bool regHover = CheckCollisionPointRec(mouse, registerBtn);
            DrawRectangleRec(registerBtn, regHover ? Theme::ButtonHover() : Theme::Primary());
            DrawText("REGISTER", 455, 408, 20, Theme::ButtonText());

            bool backHover = CheckCollisionPointRec(mouse, backBtn);
            DrawRectangleRec(backBtn, backHover ? Theme::ButtonHover() : Theme::Button());
            DrawText("BACK TO LOGIN", 410, 458, 18, Theme::ButtonText());

            if (!registerMessage.empty()) {
                DrawText(registerMessage.c_str(), 250, 500, 18, Theme::Text());
            }

            // Input handling
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, userRect)) {
                    typingRegUsername = true; typingRegEmail = false; typingRegPassword = false; typingRegConfirmPassword = false;
                } else if (CheckCollisionPointRec(mouse, emailRect)) {
                    typingRegUsername = false; typingRegEmail = true; typingRegPassword = false; typingRegConfirmPassword = false;
                } else if (CheckCollisionPointRec(mouse, passRect)) {
                    typingRegUsername = false; typingRegEmail = false; typingRegPassword = true; typingRegConfirmPassword = false;
                } else if (CheckCollisionPointRec(mouse, confRect)) {
                    typingRegUsername = false; typingRegEmail = false; typingRegPassword = false; typingRegConfirmPassword = true;
                } else if (CheckCollisionPointRec(mouse, registerBtn)) {
                    // Validate and create account
                    if (registerUsername.empty() || registerEmail.empty() || registerPassword.empty() || registerConfirmPassword.empty()) {
                        registerMessage = "Please fill in all fields";
                    } else if (!IsValidEmail(registerEmail)) {
                        registerMessage = "Invalid email (must contain @)";
                    } else if (registerPassword != registerConfirmPassword) {
                        registerMessage = "Passwords do not match";
                    } else {
                        // Save account to vector
                        std::string accountData = registerUsername + ":" + registerPassword + ":" + registerEmail;
                        accounts.push_back(accountData);
                        SaveAccountsToFile();  // Save to persistent file
                        registerMessage = "Account created! Redirecting...";
                        // Clear fields and go back to login
                        registerUsername.clear();
                        registerEmail.clear();
                        registerPassword.clear();
                        registerConfirmPassword.clear();
                        typingRegUsername = typingRegEmail = typingRegPassword = typingRegConfirmPassword = false;
                        state = LOGIN;
                    }
                } else if (CheckCollisionPointRec(mouse, backBtn)) {
                    // Go back to login
                    registerUsername.clear();
                    registerEmail.clear();
                    registerPassword.clear();
                    registerConfirmPassword.clear();
                    registerMessage = "";
                    typingRegUsername = typingRegEmail = typingRegPassword = typingRegConfirmPassword = false;
                    state = LOGIN;
                } else {
                    typingRegUsername = typingRegEmail = typingRegPassword = typingRegConfirmPassword = false;
                }
            }

            // Character input for register
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125) {
                    if (typingRegUsername && registerUsername.size() < 64) registerUsername.push_back((char)key);
                    else if (typingRegEmail && registerEmail.size() < 64) registerEmail.push_back((char)key);
                    else if (typingRegPassword && registerPassword.size() < 64) registerPassword.push_back((char)key);
                    else if (typingRegConfirmPassword && registerConfirmPassword.size() < 64) registerConfirmPassword.push_back((char)key);
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (typingRegUsername && !registerUsername.empty()) registerUsername.pop_back();
                if (typingRegEmail && !registerEmail.empty()) registerEmail.pop_back();
                if (typingRegPassword && !registerPassword.empty()) registerPassword.pop_back();
                if (typingRegConfirmPassword && !registerConfirmPassword.empty()) registerConfirmPassword.pop_back();
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

            // Recent bookings list below greeting (filtered by username)
            std::ifstream file("assets/bookings.txt");

            int y = 100;

            std::string line;
            std::string currentUsername;
            std::vector<std::string> currentBookingLines;

            while (getline(file, line)) {
                if (line.find("Username: ") == 0) {
                    currentUsername = line.substr(10); // Extract username
                    currentBookingLines.clear();
                    currentBookingLines.push_back(line);
                } else if (line.find("---") == 0) {
                    // End of booking record
                    if (currentUsername == loginUsername && currentBookingLines.size() > 0) {
                        // Display this booking (it belongs to current user)
                        std::string movieLine = "";
                        std::string bookingLine = "";
                        std::string seatsLine = "";
                        std::string totalLine = "";

                        for (const auto& l : currentBookingLines) {
                            if (l.find("Movie: ") == 0) movieLine = l;
                            if (l.find("Booking ID: ") == 0) bookingLine = l;
                            if (l.find("Seats: ") == 0) seatsLine = l;
                            if (l.find("Total Paid: ") == 0) totalLine = l;
                        }

                        if (!movieLine.empty()) {
                            Rectangle card = { 80, (float)y, 800, 120 };

                            DrawRectangleRounded(card, 0.15f, 10, LIGHTGRAY);
                            DrawRectangleRoundedLines(card, 0.15f, 10, DARKGRAY);

                            DrawText(movieLine.c_str(), 110, y + 15, 24, DARKBLUE);

                            DrawText(bookingLine.c_str(), 110, y + 50, 18, BLACK);

                            DrawText(seatsLine.c_str(), 110, y + 75, 18, BLACK);

                            DrawText(totalLine.c_str(), 550, y + 50, 20, DARKGREEN);

                            y += 140;
                        }
                    }
                    currentBookingLines.clear();
                } else if (!line.empty() && currentUsername == loginUsername) {
                    currentBookingLines.push_back(line);
                }
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

        else if (state == CART)
        {
            DrawText("Your Cart", 80, 30, 40, Theme::Primary());

            int y = 100;

            if (cart.empty()) {
                DrawText("Cart is empty", 100, 100, 24, Theme::SecondaryText());
            }

            for (int i = 0; i < (int)cart.size(); i++) {
                Booking &b = cart[i];

                Rectangle card = { 80, (float)y, 800, 100 };
                DrawRectangleRec(card, Theme::Panel());
                DrawRectangleLinesEx(card, 2, Theme::Outline());

                DrawText(b.movieTitle.c_str(), 100, y + 10, 22, Theme::Primary());
                DrawText(TextFormat("Seats: %d", (int)b.seatIds.size()), 100, y + 40, 18, Theme::Text());
                DrawText(TextFormat("Total: $%d", b.totalPrice), 740, y + 60, 22, Theme::Success());

                Rectangle buyBtn = { 650, (float)y + 10, 100, 30 };
                Rectangle removeBtn = { 760, (float)y + 10, 100, 30 };

                DrawRectangleRec(buyBtn, Theme::Primary());
                DrawText("BUY", 690, y + 18, 18, Theme::ButtonText());

                DrawRectangleRec(removeBtn, Theme::Button());
                DrawText("REMOVE", 780, y + 18, 14, Theme::ButtonText());

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mpos = GetMousePosition();
                    if (CheckCollisionPointRec(mpos, buyBtn)) {
                        // Set username before saving
                        b.username = loggedIn ? loginUsername : "Anonymous";
                        b.SaveToFile();

                        finalPrice = b.totalPrice;
                        paymentTimer = 0;
                        state = PAYMENT;

                        std::ofstream file("assets/bookings.txt", std::ios::app);
                        file << b.movieTitle << ":";

                        for (int j = 0; j < (int)b.seatIds.size(); j++) {
                            if (j) file << ",";
                            file << b.seatIds[j];
                        }
                        file << "\n";
                        file.close();


                        cart.erase(cart.begin() + i);
                        cartMessage = "Purchase successful";
                        break;
                    }

                    if (CheckCollisionPointRec(mpos, removeBtn)) {
                        cart.erase(cart.begin() + i);
                        cartMessage = "Removed from cart";
                        break;
                    }
                }

                y += 130;
            }

            Rectangle backBtn = { 400, 600, 200, 50 };
            DrawRectangleRec(backBtn, Theme::Button());
            DrawText("BACK", 470, 615, 20, Theme::ButtonText());

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(GetMousePosition(), backBtn)) {
                    state = HOME;
                }
            }
        }

        else if (state == MAIN_MENU) {

            int screenW = GetScreenWidth();

            Rectangle searchBar = {
                screenW - 20 - 300,
                70,
                300,
                40
            };

            DrawRectangleRec(
                searchBar,
                typingSearch ?
                Theme::ButtonHover() :
                Theme::Panel()
            );

            DrawRectangleLinesEx(
                searchBar,
                2,
                Theme::Outline()
            );

            DrawText(
                searchText.empty() ? "Search movie..." : searchText.c_str(),
                searchBar.x + 10,
                searchBar.y + 10,
                20,
                Theme::Text()
            );

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                typingSearch =
                    CheckCollisionPointRec(
                        GetMousePosition(),
                        searchBar
                    );
            }

            if (typingSearch)
            {
                int key = GetCharPressed();

                while (key > 0)
                {
                    if (key >= 32 && key <= 125)
                    {
                        searchText.push_back((char)key);
                    }

                    key = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE) &&
                    !searchText.empty())
                {
                    searchText.pop_back();
                }
            }

            DrawText("SiCinema", 20, 10, 40, Theme::Primary());

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

            int visibleIndex = 0;

            for (int i = 0; i < movies.size(); i++) {

                if (!searchText.empty())
                {
                    std::string title = movies[i].title;
                    std::string search = searchText;

                    std::transform(
                        title.begin(),
                        title.end(),
                        title.begin(),
                        ::tolower
                    );

                    std::transform(
                        search.begin(),
                        search.end(),
                        search.begin(),
                        ::tolower
                    );

                    if (title.find(search) ==
                        std::string::npos)
                    {
                        continue;
                    }
                }

                int x =
                    80 +
                    (visibleIndex % 3) *
                    (w + 20);

                int y =
                    120 +
                    (visibleIndex / 3) *
                    (h + 20);

                visibleIndex++;

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
                        // After adding to cart, go back to main page
                        state = HOME;
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