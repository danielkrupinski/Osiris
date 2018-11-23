#pragma once

class GUI final {
public:
    void render() noexcept;
    bool isOpen{ false };
private:
    void renderMenuBar() noexcept;
    void renderAimbotWindow() noexcept;
    void renderGlowWindow() noexcept;
    void renderChamsWindow() noexcept;
    void renderTriggerbotWindow() noexcept;
    void renderMiscWindow() noexcept;
    struct {
        bool glow{ false };
        bool chams{ false };
        bool triggerbot{ false };
        bool misc{ false };
    } window;
};

extern GUI gui;
