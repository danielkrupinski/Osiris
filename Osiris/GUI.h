#pragma once

class GUI final {
public:
    void render() noexcept;
    bool isOpen{ false };
private:
	static void checkBoxAndColorPicker(const std::string&, bool*, float*) noexcept;
    static void hotkey(int&) noexcept;
    void renderMenuBar() noexcept;
    void renderAimbotWindow() noexcept;
    void renderTriggerbotWindow() noexcept;
    void renderGlowWindow() noexcept;
    void renderChamsWindow() noexcept;
    void renderVisualsWindow() noexcept;
    void renderKnifeChangerWindow() noexcept;
    void renderMiscWindow() noexcept;
    struct {
        bool aimbot{ false };
        bool triggerbot{ false };
        bool glow{ false };
        bool chams{ false };
        bool visuals{ false };
        bool knifeChanger{ false };
        bool misc{ false };
    } window;
};

extern GUI gui;
