#pragma once
#include <string>
#include <vector>
#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"

class Movie {
public:
    std::string title;
    std::string language;
    std::string genre;
    int price;
    int duration; // minutes
    std::string rating; // letter rating like A, B, C

    Texture2D poster;

    std::vector<std::string> showTimes;

    Movie();
    Movie(std::string t, std::string l, std::string g, int p, int d, std::string r, const char* imagePath);
};
