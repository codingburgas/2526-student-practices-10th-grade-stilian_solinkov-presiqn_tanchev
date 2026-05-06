#include "../include/Movie.h"

Movie::Movie() {}

Movie::Movie(std::string t, std::string l, std::string g, int p, const char* imagePath) {
    title = t;
    language = l;
    genre = g;
    price = p;

    poster = LoadTexture(imagePath);

    if (poster.id == 0) {
        TraceLog(LOG_ERROR, "FAILED TO LOAD IMAGE: %s", imagePath);
    }
}