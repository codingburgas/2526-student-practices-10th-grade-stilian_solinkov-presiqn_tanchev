#pragma once
#include <vector>

class Booking {
public:
    int bookingId;
    std::vector<int> seatIds;

    void SaveToFile();
};