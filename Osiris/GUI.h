#pragma once

class GUI final {
public:
    void render();
    void renderGlowWindow();
    bool isOpen{ false };
};

extern GUI gui;
