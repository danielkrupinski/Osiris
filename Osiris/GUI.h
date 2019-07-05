#pragma once

#include <string>

class GUI final {
public:
    GUI() noexcept;
    void render() noexcept;
    bool isOpen{ false };
private:
    static void checkboxedColorPicker(const std::string&, bool*, float*) noexcept;
    static void hotkey(int&) noexcept;
    void updateColors() const noexcept;
    void renderMenuBar() noexcept;
    void renderAimbotWindow() noexcept;
    void renderTriggerbotWindow() noexcept;
    void renderBacktrackWindow() noexcept;
    void renderGlowWindow() noexcept;
    void renderChamsWindow() noexcept;
    void renderEspWindow() noexcept;
    void renderVisualsWindow() noexcept;
    void renderSkinChangerWindow() noexcept;
    void renderSoundWindow() noexcept;
    void renderMiscWindow() noexcept;
    void renderConfigWindow() noexcept;
    void renderGuiStyle2() noexcept;

    struct {
        bool aimbot{ false };
        bool triggerbot{ false };
        bool backtrack{ false };
        bool glow{ false };
        bool chams{ false };
        bool esp{ false };
        bool visuals{ false };
        bool skinChanger{ false };
        bool sound{ false };
        bool misc{ false };
        bool config{ false };
    } window;
};

extern GUI gui;
