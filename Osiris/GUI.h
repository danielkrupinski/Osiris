#pragma once

class GUI final {
public:
    GUI();
    void render();
    bool isOpen{ false };
};

extern GUI gui;
