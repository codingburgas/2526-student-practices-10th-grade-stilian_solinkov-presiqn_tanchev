#include "../include/Show.h"
#include <fstream>
#include <sstream>


Show::Show(Movie* m) {
    movie = m;
}

void Show::InitSeats() {
    seats.clear();

    int id = 0;
    int startX = 120;
    int startY = 260;
    int size = 40;
    int gap = 10;

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 8; c++) {

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

void Show::ConfirmBooking() {
    for (auto& s : seats)
        if (s.state == SELECTED)
            s.state = BOOKED;
}

void Show::SaveBookedSeats() {
    std::ofstream file("assets/bookings.txt", std::ios::app);

    file << movie->title << ":";

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

    while (getline(file, line)) {

        size_t sep = line.find(":");
        if (sep == std::string::npos) continue;

        std::string movieName = line.substr(0, sep);

        if (movieName != movie->title)
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