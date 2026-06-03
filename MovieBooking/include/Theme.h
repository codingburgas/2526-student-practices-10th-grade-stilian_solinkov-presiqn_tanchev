#pragma once
#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"

namespace Theme {
	enum Mode { LIGHT, DARK };

	extern Mode mode;

	void Toggle();

	Color Background();
	Color Text();
	Color SecondaryText();
	Color Primary();
	Color Button();
	Color ButtonHover();
	Color ButtonText();
	Color Panel();
	Color Outline();
	Color Success();
	Color Progress();

	// Seat specific
	Color SeatAvailable();
	Color SeatSelected();
	Color SeatBooked();
}
