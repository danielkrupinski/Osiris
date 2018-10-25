#pragma once

class GUI final {
public:
    void render();
    bool isOpen{ false };
};

extern GUI gui;
