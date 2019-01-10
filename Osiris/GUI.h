#pragma once

class GUI final {
public:
    void render() noexcept;
    bool isOpen{ false };
private:
	void checkBoxAndColorPicker(const std::string_view, bool*, float*) const noexcept;
    void renderMenuBar() noexcept;
    void renderAimbotWindow() noexcept;
    void renderTriggerbotWindow() noexcept;
    void renderGlowWindow() noexcept;
    void renderChamsWindow() noexcept;
    void renderMiscWindow() noexcept;
    struct {
        bool aimbot{ false };
        bool glow{ false };
        bool chams{ false };
        bool triggerbot{ false };
        bool misc{ false };
    } window;
};

extern GUI gui;
