#pragma once
#include <vector>
#include <string>
#include "Seat.h"
#include "Movie.h"

class Show {
public:
    Movie* movie;
    std::string showTime;
    std::vector<Seat> seats;

    Show();
    Show(Movie* m);
    Show(Movie* m, const std::string& time);

    void InitSeats();
    void Update();
    void Draw();

    int GetTotalPrice();
    std::string GetBookingKey() const;
    void ConfirmBooking();
    void SaveBookedSeats();
    void LoadBookedSeats();
};
