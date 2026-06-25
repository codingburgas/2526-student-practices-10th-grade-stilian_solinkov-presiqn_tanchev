#include "../include/Movie.h"

Movie::Movie() {}

Movie::Movie(std::string t, std::string l, std::string g, int p, int d, std::string r, const char* imagePath) {
    title = t;
    language = l;
    genre = g;
    price = p;
    duration = d;
    rating = r;

    poster = LoadTexture(imagePath);

    if (poster.id == 0) {
        TraceLog(LOG_ERROR, "FAILED TO LOAD IMAGE: %s", imagePath);
    }
}
