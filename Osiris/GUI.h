#pragma once

#include <memory>
#include <string>

struct ImFont;

class GUI {
public:
    GUI() noexcept;
    void render() noexcept;
    bool open = true;
private:
    static void hotkey(int&) noexcept;
    void updateColors() const noexcept;
    void drawLuaItems(int tab, int column);
    void renderAimbotWindow(bool contentOnly = false) noexcept;
    void renderAntiAimWindow(bool contentOnly = false) noexcept;
    void renderTriggerbotWindow(bool contentOnly = false) noexcept;
    void renderBacktrackWindow(bool contentOnly = false) noexcept;
    void renderGlowWindow(bool contentOnly = false) noexcept;
    void renderChamsWindow(bool contentOnly = false) noexcept;
    void renderEspWindow(bool contentOnly = false) noexcept;
    void renderVisualsWindow(bool contentOnly = false) noexcept;
    void renderSkinChangerWindow(bool contentOnly = false) noexcept;
    void renderSoundWindow(bool contentOnly = false) noexcept;
    void renderStyleWindow(bool contentOnly = false) noexcept;
    void renderMiscWindow(bool contentOnly = false) noexcept;
    void renderReportbotWindow(bool contentOnly = false) noexcept;
    void renderConfigWindow(bool contentOnly = false) noexcept;
    void renderAutoConfigWindow(bool contentOnly = false) noexcept;
    void renderaimhacksWindow(bool contentOnly = false) noexcept;
    void renderwallhacksWindow(bool contentOnly = false) noexcept;
    void renderotherhacksWindow(bool contentOnly = false) noexcept;
    void renderPopupMenu(bool contentOnly = false) noexcept;
    void renderskinchangersWindow(bool contentOnly = false) noexcept;
    void renderPlayerWindow(bool contentOnly = false) noexcept;
    void renderGuiStyle2() noexcept;

    struct {
        bool aimbot = false;
        bool antiAim = false;
        bool triggerbot = false;
        bool backtrack = false;
        bool glow = false;
        bool chams = false;
        bool esp = false;
        bool visuals = false;
        bool skinChanger = false;
        bool sound = false;
        bool style = false;
        bool misc = false;
        bool reportbot = false;
        bool config = false;
        bool autoconfig = false;
        bool aimhacks = false;
        bool wallhacks = false;
        bool other = false;
        bool popupMenu = true;
        bool SkinChangers = false;
        bool playerChanger = false;
    } window;

    struct {
        ImFont* tahoma = nullptr;
        ImFont* segoeui = nullptr;
        ImFont* arial = nullptr;
    } fonts;
};

inline std::unique_ptr<GUI> gui;
