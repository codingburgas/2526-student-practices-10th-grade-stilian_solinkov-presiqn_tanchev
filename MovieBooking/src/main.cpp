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
    ACCOUNT,
    ADMIN_PANEL
};

AppState state = HOME;
AppState prevState = HOME;
bool loggedIn = false;
enum UserRole
{
    USER,
    ADMIN
};

UserRole currentRole = USER;
float adminScroll = 0;
std::vector<Booking> cart;
std::string cartMessage;
std::string selectedTime = "";
int selectedAdminMovie = -1;

int selectedMovie = -1;
Show* currentShow = nullptr;

int finalPrice = 0;
float paymentTimer = 0;

std::vector<Movie> movies;
std::string searchText;
bool typingSearch = false;
float movieScroll = 0.0f;

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

// Login validation function - checks if username and password match a registered account
bool ValidateLogin(const std::string& username, const std::string& password) {
    if (username == "admin" && password == "admin123")
    {
        currentRole = ADMIN;
        return true;
    }
    for (const auto& account : accounts) {
        // Account format: username:password:email
        size_t firstColon = account.find(':');
        size_t secondColon = account.find(':', firstColon + 1);

        if (firstColon != std::string::npos && secondColon != std::string::npos) {
            std::string accUsername = account.substr(0, firstColon);
            std::string accPassword = account.substr(firstColon + 1, secondColon - firstColon - 1);

            if (accUsername == username && accPassword == password) {
                return true; // Valid credentials
            }
        }
    }
    return false; // Invalid credentials
}

// Check if username already exists
bool UsernameExists(const std::string& username) {
    for (const auto& account : accounts) {
        size_t firstColon = account.find(':');
        if (firstColon != std::string::npos) {
            std::string accUsername = account.substr(0, firstColon);
            if (accUsername == username) {
                return true; // Username exists
            }
        }
    }
    return false; // Username doesn't exist
}

// Check if email already exists
bool EmailExists(const std::string& email) {
    for (const auto& account : accounts) {
        size_t secondColon = account.rfind(':');
        if (secondColon != std::string::npos) {
            std::string accEmail = account.substr(secondColon + 1);
            if (accEmail == email) {
                return true; // Email exists
            }
        }
    }
    return false; // Email doesn't exist
}

void InitMovies() {

    movies.push_back(Movie("Interstellar", "EN", "Sci-Fi", 12, 169, "A", "assets/images/interstellar.png"));
    movies.back().showTimes = { "14:00", "18:00", "21:30" };

    movies.push_back(Movie("Avatar 2", "EN", "Action", 15, 192, "B", "assets/images/avatar.png"));
    movies.back().showTimes = { "13:00", "17:00", "20:30" };

    movies.push_back(Movie("Oppenheimer", "EN", "Drama", 18, 180, "A", "assets/images/oppenheimer.png"));
    movies.back().showTimes = { "15:00", "19:00", "22:00" };

    movies.push_back(Movie("The Dark Knight", "EN", "Action", 13, 152, "A", "assets/images/batman.png"));
    movies.back().showTimes = { "16:00", "19:00", "22:30" };

    movies.push_back(Movie("Inception", "EN", "Sci-Fi", 14, 148, "A", "assets/images/inception.png"));
    movies.back().showTimes = { "12:30", "15:30", "20:00" };

    movies.push_back(Movie("Joker", "EN", "Drama", 11, 122, "B", "assets/images/joker.png"));
    movies.back().showTimes = { "13:00", "18:00", "21:00" };

    movies.push_back(Movie("Titanic", "EN", "Romance", 9, 195, "B", "assets/images/titanic.png"));
    movies.back().showTimes = { "11:00", "14:00", "17:00" };

    movies.push_back(Movie("Dune", "EN", "Sci-Fi", 16, 155, "B", "assets/images/dune.png"));
    movies.back().showTimes = { "14:15", "19:15", "22:15" };

    movies.push_back(Movie("Matrix", "EN", "Action", 13, 136, "A", "assets/images/matrix.png"));
    movies.back().showTimes = { "12:00", "16:00", "20:00" };

    movies.push_back(Movie("Pulp Fiction", "EN", "Crime", 10, 154, "B", "assets/images/joker.png"));
    movies.back().showTimes = { "15:00", "19:30" };

    movies.push_back(Movie("The Shawshank Redemption", "EN", "Drama", 12, 142, "A", "assets/images/titanic.png"));
    movies.back().showTimes = { "13:30", "18:30" };

    movies.push_back(Movie("Parasite", "EN", "Thriller", 11, 132, "A", "assets/images/matrix.png"));
    movies.back().showTimes = { "17:00", "20:30" };
}

// UI helper functions: rounded buttons and panels
static void DrawRoundedButton(Rectangle rect, const char* label, Font font, int fontSize, Color bg, Color textColor) {
    float round = 0.25f;
    int segments = 8;
    DrawRectangleRounded(rect, round, segments, bg);
    // removed border lines for navbar buttons
    // DrawRectangleRoundedLines(rect, round, segments, Theme::Outline());
    Vector2 textSize = MeasureTextEx(font, label, fontSize, 1.0f);
    Vector2 pos = { rect.x + (rect.width - textSize.x) / 2.0f, rect.y + (rect.height - textSize.y) / 2.0f };
    DrawTextEx(font, label, pos, fontSize, 1.0f, textColor);
}

static void DrawRoundedPanel(Rectangle rect, Color bg) {
    DrawRectangleRounded(rect, 0.12f, 6, bg);
}

int main() {

    InitWindow(1000, 700, "SiCinema");
    SetTargetFPS(60);

    // Load application font (fallback to default)
    Font appFont = GetFontDefault();
    if (FileExists("assets/fonts/Roboto-Regular.ttf")) {
        appFont = LoadFontEx("assets/fonts/Roboto-Regular.ttf", 32, NULL, 0);
    }

    InitMovies();
    LoadAccountsFromFile();  // Load saved accounts at startup

    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(Theme::Background());

        // Global navbar on all pages except seat-booking (BOOKING)
        int screenW = GetScreenWidth();
        if (state != BOOKING) {
            Rectangle navBar = { 0, 0, (float)screenW, 70 };
            DrawRoundedPanel(navBar, Theme::NavBar());

            int navBtnW = 110; int navBtnH = 40; int navSpacing = 12;
            Rectangle navThemeBtn = { (float)(screenW - 20 - navBtnW), 15, (float)navBtnW, (float)navBtnH };
            Rectangle navCartBtn = { (float)(navThemeBtn.x - navSpacing - navBtnW), 15, (float)navBtnW, (float)navBtnH };
            Rectangle navLoginBtn = { (float)(navCartBtn.x - navSpacing - navBtnW), 15, (float)navBtnW, (float)navBtnH };

            // Brand on left
            DrawTextEx(appFont, "SiCinema", Vector2{20.0f, 18.0f}, 30.0f, 2.0f, Theme::ButtonText());

            // Admin button next to brand if admin
            if (currentRole == ADMIN) {
                Rectangle navAdminBtn = { 160.0f, 15.0f, 120.0f, 40.0f };
                bool navAdminHover = CheckCollisionPointRec(mouse, navAdminBtn);
                DrawRoundedButton(navAdminBtn, "Admin", appFont, 18, navAdminHover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, navAdminBtn)) state = ADMIN_PANEL;
            }

            bool navLoginHover = CheckCollisionPointRec(mouse, navLoginBtn);
            DrawRoundedButton(navLoginBtn, (!loggedIn) ? "Login" : (state == ACCOUNT ? "Logout" : "Account"), appFont, 18, navLoginHover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());

            bool navCartHover = CheckCollisionPointRec(mouse, navCartBtn);
            DrawRoundedButton(navCartBtn, TextFormat("Cart (%d)", (int)cart.size()), appFont, 18, navCartHover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());

            bool navThemeHover = CheckCollisionPointRec(mouse, navThemeBtn);
            DrawRoundedButton(navThemeBtn, "Theme", appFont, 18, navThemeHover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, navLoginBtn)) {
                    if (!loggedIn) state = LOGIN;
                    else {
                        if (state == ACCOUNT) {
                            loggedIn = false; loginUsername.clear(); state = HOME;
                        } else state = ACCOUNT;
                    }
                }
                if (CheckCollisionPointRec(mouse, navCartBtn)) {
                    state = CART;
                }
                if (CheckCollisionPointRec(mouse, navThemeBtn)) {
                    Theme::Toggle();
                }
            }
        }

        if (state == LOGIN) {

            // Draw login screen
            DrawText("Login", 420, 120, 48, Theme::Primary());

            Rectangle userRect = { 300, 220, 400, 40 };
            Rectangle passRect = { 300, 280, 400, 40 };
            Rectangle loginBtn = { 430, 340, 140, 40 };
            Rectangle backBtn = { 350, 600, 250, 60 };


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

            DrawRoundedButton(loginBtn, "LOGIN", appFont, 20, loginHover ? Theme::ButtonHover() : Theme::Primary(), Theme::ButtonText());

            DrawRoundedButton(backBtn, "BACK", appFont, 25, Theme::Button(), Theme::ButtonText());

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(GetMousePosition(), backBtn)) {
                    state = HOME;
                }
            }

            // Create Account button
            Rectangle createBtn = { 430, 400, 140, 40 };
            bool createHover = CheckCollisionPointRec(mouse, createBtn);
            DrawRoundedButton(createBtn, "CREATE ACCOUNT", appFont, 16, createHover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());

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
                    } else if (ValidateLogin(loginUsername, loginPassword)) {
                        loginMessage = "Login successful";
                        typingUsername = typingPassword = false;
                        state = HOME;
                        loggedIn = true;
                    } else {
                        loginMessage = "Invalid username/password";
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
                } else if (ValidateLogin(loginUsername, loginPassword)) {
                    loginMessage = "Login successful";
                    typingUsername = typingPassword = false;
                    state = HOME;
                    loggedIn = true;
                } else {
                    loginMessage = "Invalid username/password";
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
            DrawRoundedButton(registerBtn, "REGISTER", appFont, 20, regHover ? Theme::ButtonHover() : Theme::Primary(), Theme::ButtonText());

            bool backHover = CheckCollisionPointRec(mouse, backBtn);
            DrawRoundedButton(backBtn, "BACK TO LOGIN", appFont, 18, backHover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());

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
                    } else if (UsernameExists(registerUsername)) {
                        registerMessage = "Already taken";
                    } else if (EmailExists(registerEmail)) {
                        registerMessage = "Already taken";
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

            // Movie grid shown directly on HOME (navbar remains)
            DrawTextEx(appFont, "Movie Ticket Booking System", Vector2{300.0f, 160.0f}, 25.0f, 1.0f, Theme::SecondaryText());

            // Search bar under navbar - use contrasting background so it's always visible
            Rectangle searchBar = { screenW - 20 - 300, 80, 300, 40 };
            Color searchBg = (Theme::mode == Theme::DARK) ? Color{30, 60, 120, 255} : Theme::Panel();
            DrawRoundedPanel(searchBar, typingSearch ? Theme::ButtonHover() : searchBg);
            DrawTextEx(appFont, searchText.empty() ? "Search movie..." : searchText.c_str(), Vector2{ searchBar.x + 10.0f, searchBar.y + 8.0f }, 20.0f, 1.0f, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                typingSearch = CheckCollisionPointRec(GetMousePosition(), searchBar);
            }

            if (typingSearch) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (key >= 32 && key <= 125) searchText.push_back((char)key);
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && !searchText.empty()) searchText.pop_back();
            }

            int w = 840; int h = 160; int visibleIndex = 0; int startY = 220; int gap = 20;

                                    // Build filtered list based on search
                                    std::vector<int> visibleIndices;
                                    for (int i = 0; i < (int)movies.size(); i++) {
                                        if (!searchText.empty()) {
                                            std::string title = movies[i].title;
                                            std::string search = searchText;
                                            std::transform(title.begin(), title.end(), title.begin(), ::tolower);
                                            std::transform(search.begin(), search.end(), search.begin(), ::tolower);
                                            if (title.find(search) == std::string::npos) continue;
                                        }
                                        visibleIndices.push_back(i);
                                    }

                                    // Vertical scroll handling (only when mouse over list area)
                                    float totalHeight = (h + gap) * (float)visibleIndices.size();
                                    float viewHeight = (float)(GetScreenHeight() - startY - 20);
                                    if (totalHeight > viewHeight) {
                                        if (mouse.y >= startY && mouse.y <= GetScreenHeight()) {
                                            float wheel = GetMouseWheelMove();
                                            movieScroll -= wheel * 30.0f;
                                        }
                                        if (movieScroll < 0) movieScroll = 0;
                                        float maxScroll = std::max(0.0f, totalHeight - viewHeight);
                                        if (movieScroll > maxScroll) movieScroll = maxScroll;
                                    } else {
                                        movieScroll = 0;
                                    }

                                    // Clip drawing to content area so navbar/searchbar stay on top
                                    BeginScissorMode(0, startY, GetScreenWidth(), GetScreenHeight() - startY);

                                    for (int idx = 0; idx < (int)visibleIndices.size(); idx++) {
                                        int i = visibleIndices[idx];

                                        int x = 80;
                                        int y = startY + idx * (h + gap) - (int)movieScroll;

                                        Rectangle card = { (float)x, (float)y, (float)w, (float)h };
                                        bool hover = CheckCollisionPointRec(mouse, card);
                                        // Card background match page background
                                        DrawRoundedPanel(card, Theme::Background());

                                        // Top border only (no full border) - gray color
                                        Color topBorderColor = Color{200,200,200,255};
                                        DrawRectangle((int)card.x, (int)card.y, (int)card.width, 4, topBorderColor);

                                        // Poster on left as portrait with a bit of top margin
                                        float posterMarginLeft = 10.0f;
                                        float posterMarginTop = 12.0f;
                                        float posterBottomMargin = 10.0f;
                                        float posterW = 120.0f;
                                        float posterH = (float)h - posterMarginTop - posterBottomMargin;

                                        // Poster background (white)
                                        DrawRectangle((int)(x + posterMarginLeft), (int)(y + posterMarginTop), (int)posterW, (int)posterH, WHITE);
                                        DrawTexturePro(movies[i].poster, {0,0,(float)movies[i].poster.width,(float)movies[i].poster.height}, { (float)x + posterMarginLeft, (float)y + posterMarginTop, posterW, posterH }, {0,0}, 0, WHITE);

                                        // Rating circle on poster (letter inside)
                                        float ocx = (float)x + posterMarginLeft + posterW - 18.0f;
                                        float ocy = (float)y + posterMarginTop + 18.0f;
                                        DrawCircle((int)ocx, (int)ocy, 16, Theme::Button());
                                        DrawCircleLines((int)ocx, (int)ocy, 16, Theme::Outline());
                                        // Draw rating letter
                                        DrawTextEx(appFont, movies[i].rating.c_str(), Vector2{ocx - 7.0f, ocy - 10.0f}, 20.0f, 1.0f, WHITE);

                                        // Details on right
                                        Vector2 titlePos = { (float)x + 160, (float)y + 10 };
                                        Vector2 metaPos = { (float)x + 160, (float)y + 46 };
                                        Vector2 showtimesStart = { (float)x + 160, (float)y + 78 };
                                        Vector2 langPos = { (float)x + 160, (float)y + 118 };
                                        Vector2 pricePos = { (float)x + 600, (float)y + 60 };

                                        // Title (prominent)
                                        DrawTextEx(appFont, movies[i].title.c_str(), titlePos, 26.0f, 1.0f, Theme::Primary());

                                        // Small meta line: genre and duration (no icon)
                                        Color metaColor = Theme::Text();
                                        DrawTextEx(appFont, TextFormat("%s | %d min.", movies[i].genre.c_str(), movies[i].duration), metaPos, 18.0f, 1.0f, metaColor);

                                        // Showtimes as orange rounded buttons (match movie tab)
                                        Color showtimeColor = Color{236,125,26,255};
                                        int btnW = 90; int btnH = 38; int btnGap = 12;
                                        for (int s = 0; s < (int)movies[i].showTimes.size(); s++) {
                                            float sx = showtimesStart.x + s * (btnW + btnGap);
                                            float sy = showtimesStart.y;
                                            Rectangle stRect = { sx, sy, (float)btnW, (float)btnH };
                                            bool stHover = CheckCollisionPointRec(mouse, stRect);
                                            DrawRoundedButton(stRect, movies[i].showTimes[s].c_str(), appFont, 18, stHover ? showtimeColor : showtimeColor, WHITE);

                                            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, stRect)) {
                                                // select this movie and time and go to booking
                                                selectedMovie = i;
                                                selectedTime = movies[i].showTimes[s];
                                                currentShow = new Show(&movies[i]);
                                                currentShow->InitSeats();
                                                currentShow->LoadBookedSeats();
                                                prevState = state;
                                                state = BOOKING;
                                            }
                                        }

                                        // Language or subtitle info
                                        DrawTextEx(appFont, TextFormat("%s", movies[i].language.c_str()), langPos, 16.0f, 1.0f, Theme::SecondaryText());

                                        // Price on the right
                                        DrawTextEx(appFont, TextFormat("%d$", movies[i].price), pricePos, 24.0f, 1.0f, Theme::Primary());

                                        // Card click fallback (clicking elsewhere opens booking)
                                        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                            selectedMovie = i;
                                            currentShow = new Show(&movies[i]);
                                            currentShow->InitSeats();
                                            currentShow->LoadBookedSeats();
                                            prevState = state;
                                            state = BOOKING;
                                        }
                                    }

                                    EndScissorMode();

        }

        else if (state == ACCOUNT)
        {
            // Greeting (moved down to avoid navbar overlap at y=70)
            std::string greet = "Hello " + loginUsername;
            DrawText(greet.c_str(), 80, 100, 40, Theme::Primary());

            // Recent bookings list below greeting (filtered by username)
            std::ifstream file("assets/bookings.txt");

            int y = 160;

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

            Rectangle backBtn = { 400, 600, 200, 50 };

            DrawRoundedButton(backBtn, "BACK", appFont, 20, GRAY, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(GetMousePosition(), backBtn))
                {
                    state = HOME;
                }
            }
        }

        else if (state == CART)
        {
            // Title moved down to avoid navbar overlap
            DrawText("Your Cart", 80, 100, 40, Theme::Primary());

            int y = 160;

            if (cart.empty()) {
                DrawText("Cart is empty", 100, 160, 24, Theme::SecondaryText());
            }

            for (int i = 0; i < (int)cart.size(); i++) {
                Booking &b = cart[i];

                Rectangle card = { 80, (float)y, 800, 100 };
                DrawRoundedPanel(card, Theme::Panel());

                DrawText(b.movieTitle.c_str(), 100, y + 10, 22, Theme::Primary());
                DrawText(TextFormat("Seats: %d", (int)b.seatIds.size()), 100, y + 40, 18, Theme::Text());
                DrawText(TextFormat("Total: $%d", b.totalPrice), 740, y + 60, 22, Theme::Success());

                Rectangle buyBtn = { 650, (float)y + 10, 100, 30 };
                Rectangle removeBtn = { 760, (float)y + 10, 100, 30 };

                DrawRoundedButton(buyBtn, "BUY", appFont, 18, Theme::Primary(), Theme::ButtonText());

                DrawRoundedButton(removeBtn, "REMOVE", appFont, 14, Theme::Button(), Theme::ButtonText());

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

            Rectangle backBtn = { 400, 620, 200, 50 };
            DrawRoundedButton(backBtn, "BACK", appFont, 20, Theme::Button(), Theme::ButtonText());

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

            DrawRoundedPanel(searchBar, typingSearch ? Theme::ButtonHover() : Theme::Panel());

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

            DrawRoundedButton(backBtn, "BACK", appFont, 25, Theme::Button(), Theme::ButtonText());

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



                DrawTexturePro(
                    movies[i].poster,
                    { 0,0,(float)movies[i].poster.width,(float)movies[i].poster.height },
                    { (float)x + 10,(float)y + 10,80,100 },
                    { 0,0 },
                    0,
                    WHITE
                );

                DrawTextEx(appFont, movies[i].title.c_str(), Vector2{ (float)(x + 100), (float)(y + 10) }, 20.0f, 1.0f, Theme::ButtonText());
                DrawTextEx(appFont, TextFormat("Price: %d$", movies[i].price), Vector2{ (float)(x + 100), (float)(y + 50) }, 20.0f, 1.0f, Theme::ButtonText());

                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedMovie = i;
                    currentShow = new Show(&movies[i]);
                    currentShow->InitSeats();

                    currentShow->LoadBookedSeats();

                    prevState = state;
                    state = BOOKING;
                }
            }
        }

        else if (state == BOOKING) {

            DrawText(movies[selectedMovie].title.c_str(), 20, 20, 30, Theme::Primary());
            DrawText(TextFormat("Price: %d$", movies[selectedMovie].price), 20, 60, 25, Theme::Success());
            DrawText(TextFormat("Total: %d$", currentShow->GetTotalPrice()), 20, 100, 25, Theme::Text());
            DrawText(
                selectedTime.empty() ? "No time selected" : selectedTime.c_str(),
                20, 230, 20, Theme::Primary()
            );
            DrawText("Select Time:", 20, 140, 20, Theme::Text());

            int x = 20;
            int y = 160;

            for (int i = 0; i < movies[selectedMovie].showTimes.size(); i++)
            {
                Rectangle btn = { (float)x, (float)y, 100, 40 };

                bool hover = CheckCollisionPointRec(mouse, btn);

                DrawRoundedButton(btn, movies[selectedMovie].showTimes[i].c_str(), appFont, 18, hover ? Theme::ButtonHover() : Theme::Button(), Theme::ButtonText());

                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    selectedTime = movies[selectedMovie].showTimes[i];
                }

                x += 120;
            }

            int topY = 260;      // buttons
            int timesY = 160;    // showtimes

            Rectangle payBtn = { 20, topY, 200, 50 };
            Rectangle backBtn = { 240, topY, 200, 50 };

            DrawRoundedButton(payBtn, "ADD TO CART", appFont, 20, Theme::Primary(), Theme::ButtonText());

            DrawRoundedButton(backBtn, "BACK", appFont, 20, Theme::Button(), Theme::ButtonText());

            if (currentShow != nullptr) {
                currentShow->Update();
                currentShow->Draw();
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(mouse, payBtn)) {
                    // Add selected seats to cart (anonymous allowed)
                    Booking b;
                    b.bookingId = rand() % 10000;
                    b.movieTitle = movies[selectedMovie].title + " (" + selectedTime + ")";
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

                    state = HOME;
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

            DrawRoundedButton(backBtn, "BACK TO MENU", appFont, 20, Theme::Primary(), Theme::ButtonText());

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

                                state = HOME;
                            }
                        }
        }
        else if (state == ADMIN_PANEL)
        {
            DrawTextEx(appFont, "ADMIN PANEL", Vector2{50.0f, 90.0f}, 40.0f, 1.0f, Theme::Primary());

            Rectangle adminBackBtn = { 750, 80, 120, 40 };
            DrawRoundedButton(adminBackBtn, "BACK", appFont, 20, Theme::Button(), Theme::ButtonText());
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, adminBackBtn)) { state = HOME; }

            adminScroll -= GetMouseWheelMove() * 30;

            int y = 150 + (int)adminScroll;

            for (int i = 0; i < movies.size(); i++)
            {
                DrawTextEx(appFont, movies[i].title.c_str(), Vector2{50.0f, (float)y}, 25.0f, 1.0f, Theme::Text());

                DrawTextEx(appFont, TextFormat("Price: %d$", movies[i].price), Vector2{250.0f, (float)y}, 25.0f, 1.0f, Theme::Text());

                Rectangle plusBtn = { 450, (float)y, 40, 40 };
                Rectangle minusBtn = { 500, (float)y, 40, 40 };
                Rectangle deleteBtn = { 560, (float)y, 120, 40 };

                DrawRoundedButton(plusBtn, "+", appFont, 25, GREEN, WHITE);
                DrawRoundedButton(minusBtn, "-", appFont, 25, ORANGE, WHITE);
                DrawRoundedButton(deleteBtn, "DELETE", appFont, 18, RED, WHITE);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (CheckCollisionPointRec(mouse, plusBtn)) { movies[i].price++; }
                    if (CheckCollisionPointRec(mouse, minusBtn)) { if (movies[i].price > 1) movies[i].price--; }
                    if (CheckCollisionPointRec(mouse, deleteBtn)) { UnloadTexture(movies[i].poster); movies.erase(movies.begin() + i); break; }
                }

                y += 70;
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