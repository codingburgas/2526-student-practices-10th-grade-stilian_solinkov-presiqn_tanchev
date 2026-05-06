#pragma once
#include <vector>
#include "Seat.h"
#include "Movie.h"

class Show {
public:
    Movie* movie;
    std::vector<Seat> seats;

    Show();
    Show(Movie* m);

    void InitSeats();
    void Update();
    void Draw();

    int GetTotalPrice();
    void ConfirmBooking();
    void SaveBookedSeats();
};