#include "../include/Booking.h"
#include <fstream>

void Booking::SaveToFile()
{
    std::ofstream file("assets/bookings.txt", std::ios::app);

    file << "Booking ID: " << bookingId << "\n";
    file << "Movie: " << movieTitle << "\n";

    file << "Seats: ";

    for (int id : seatIds)
        file << id << " ";

    file << "\n";

    file << "Total Paid: $" << totalPrice << "\n";
    file << "----------------------\n";

    file.close();
}