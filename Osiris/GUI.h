#pragma once

#include <thread>
#include <d3d9helper.h>

class GUI final {
public:
    GUI();
    void render();
    bool isOpen{ false };
};

extern GUI gui;
