#pragma once

class GUI final {
public:
    void render();
    void renderGlowWindow();
    bool isOpen{ false };
private:
    struct {
        bool glow{ false };
        bool chams{ false };
        bool triggerbot{ false };
        bool misc{ false };
    } window;
};

extern GUI gui;
