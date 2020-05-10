#pragma once

#include <memory>
#include <string>

struct ImFont;
class GUI {


public:
    GUI() noexcept;
    void render() noexcept;
    bool open = false;
    static void hotkey(int&) noexcept;
private:
    void renderMenuBar() noexcept;
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
    } window;

    struct {
        ImFont* tahoma = nullptr;
        ImFont* segoeui = nullptr;
    } fonts;
};

inline std::unique_ptr<GUI> gui;
