#pragma once
#include "Show.h"

class UI {
public:
    Show* show;

    void Init(Show* s);
    void Update();
    void Draw();
};