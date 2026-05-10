#pragma once
#include <string>
#include "../external/raylib-5.5_win64_msvc16/include/raylib.h"

class Movie {
public:
    std::string title;
    std::string language;
    std::string genre;
    int price;

    Texture2D poster;

    Movie();
    Movie(std::string t, std::string l, std::string g, int p, const char* imagePath);
};