#pragma once

class GUI final {
public:
    void render();
    bool isOpen{ false };
private:
    void renderGlowWindow();
    void renderChamsWindow();
    void renderTriggerbotWindow();
    void renderMiscWindow();
    struct {
        bool glow{ false };
        bool chams{ false };
        bool triggerbot{ false };
        bool misc{ false };
    } window;
};

extern GUI gui;
