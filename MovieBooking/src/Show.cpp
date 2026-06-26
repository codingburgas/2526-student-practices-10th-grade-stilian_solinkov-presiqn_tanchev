#include "../include/Show.h"
#include <fstream>
#include <sstream>


Show::Show() {
    movie = nullptr;
}

Show::Show(Movie* m) {
    movie = m;
}

Show::Show(Movie* m, const std::string& time) {
    movie = m;
    showTime = time;
}

void Show::InitSeats() {
    seats.clear();

    int id = 0;
    int startX = 70;
    int startY = 360;
    int size = 40;
    int gap = 10;

    // Create cinema with two aisles: skip columns 2 and 5
    // Last row (row 4) has all columns
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 8; c++) {
            // Skip columns 2 and 5 for rows 0-3, but include for last row
            if (r < 4 && (c == 2 || c == 5)) continue;

            Rectangle rect = {
                startX + c * (size + gap),
                startY + r * (size + gap),
                (float)size,
                (float)size
            };

            seats.push_back(Seat(id++, rect));
        }
    }
}

void Show::Update() {
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (auto& s : seats) {
            if (s.IsClicked(mouse) && s.state != BOOKED) {

                if (s.state == AVAILABLE)
                    s.state = SELECTED;
                else if (s.state == SELECTED)
                    s.state = AVAILABLE;
            }
        }
    }
}

void Show::Draw() {
    // Draw staircases/aisles in both middle sections (columns 2 and 5) for rows 0-3
    int startX = 70;
    int startY = 360;
    int size = 40;
    int gap = 10;
    int aisle1_x = startX + 2 * (size + gap);
    int aisle2_x = startX + 5 * (size + gap);

    // Draw staircase steps for rows 0-3 in both aisles
    for (int r = 0; r < 4; r++) {
        int y = startY + r * (size + gap);

        // Left aisle (column 2)
        DrawRectangle(aisle1_x + 2, y + 5 + (r % 2) * 5, size - 4, 10, Color{150, 150, 150, 200});
        DrawRectangle(aisle1_x + 5, y + 15 + (r % 2) * 5, size - 10, 8, Color{200, 200, 200, 200});
        DrawRectangleLines(aisle1_x, y, size, size, Color{100, 100, 100, 100});

        // Right aisle (column 5)
        DrawRectangle(aisle2_x + 2, y + 5 + (r % 2) * 5, size - 4, 10, Color{150, 150, 150, 200});
        DrawRectangle(aisle2_x + 5, y + 15 + (r % 2) * 5, size - 10, 8, Color{200, 200, 200, 200});
        DrawRectangleLines(aisle2_x, y, size, size, Color{100, 100, 100, 100});
    }

    // Draw all seats
    for (auto& s : seats)
        s.Draw();
}

int Show::GetTotalPrice() {
    int total = 0;

    for (auto& s : seats)
        if (s.state == SELECTED)
            total += movie->price;

    return total;
}

std::string Show::GetBookingKey() const {
    if (movie == nullptr) return "";
    return movie->title + "|" + showTime;
}

void Show::ConfirmBooking() {
    for (auto& s : seats)
        if (s.state == SELECTED)
            s.state = BOOKED;
}

void Show::SaveBookedSeats() {
    std::ofstream file("assets/bookings.txt", std::ios::app);

    file << GetBookingKey() << ":";

    bool first = true;

    for (auto& s : seats) {
        if (s.state == BOOKED) {
            if (!first) file << ",";
            file << s.id;
            first = false;
        }
    }

    file << "\n";

    file.close();
}

void Show::LoadBookedSeats() {

    std::ifstream file("assets/bookings.txt");
    std::string line;
    std::string bookingKey = GetBookingKey();

    while (getline(file, line)) {

        size_t sep = line.rfind(":");
        if (sep == std::string::npos) continue;

        std::string savedKey = line.substr(0, sep);

        if (savedKey != bookingKey)
            continue;

        std::string seatData = line.substr(sep + 1);

        std::stringstream ss(seatData);
        std::string idStr;

        while (getline(ss, idStr, ',')) {

            if (idStr.empty()) continue;

            int id = std::stoi(idStr);

            for (auto& s : seats) {
                if (s.id == id)
                    s.state = BOOKED;
            }
        }
    }

    file.close();
}
