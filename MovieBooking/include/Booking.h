#pragma once
#include <vector>
#include <string>

class Booking {
public:
    int bookingId;
    std::string movieTitle;
    int totalPrice;
    std::string username;  // Track which user made this booking

    std::vector<int> seatIds;

    void SaveToFile();
};