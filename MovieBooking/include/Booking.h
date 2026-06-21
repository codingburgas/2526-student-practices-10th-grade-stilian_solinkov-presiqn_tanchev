#pragma once
#include <vector>
#include <string>

class Booking {
public:
    int bookingId;
    std::string movieTitle;
    int totalPrice;

    std::vector<int> seatIds;

    void SaveToFile();
};