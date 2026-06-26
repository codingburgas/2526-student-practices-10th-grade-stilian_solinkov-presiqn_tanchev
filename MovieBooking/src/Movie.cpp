#include "../include/Movie.h"

Movie::Movie() {}

Movie::Movie(std::string t, std::string l, std::string g, int p, int d, std::string r, std::string path) {
    title = t;
    language = l;
    genre = g;
    price = p;
    duration = d;
    rating = r;
    imagePath = path;

    poster = LoadTexture(path.c_str());

    if (poster.id == 0) {
        TraceLog(LOG_ERROR, "FAILED TO LOAD IMAGE: %s", path.c_str());
    }
}
