#include "../include/Booking.h"
#include <fstream>

void Booking::SaveToFile() {
    std::ofstream file("data/bookings.txt", std::ios::app);

    file << "Booking ID: " << bookingId << "\nSeats: ";

    for (int id : seatIds)
        file << id << " ";

    file << "\n----------------------\n";

    file.close();
}