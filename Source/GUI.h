#pragma once

#include <optional>

#include "Config.h"
#include "Memory.h"
#include "InventoryChanger/InventoryChanger.h"

class Visuals;
struct ImFont;
class ClientInterfaces;
class Hooks;
class Misc;

class GUI {
public:
    GUI() noexcept;
    void render(Hooks& hooks, const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config) noexcept;
    void handleToggle(Misc& misc, const OtherInterfaces& interfaces) noexcept;
    [[nodiscard]] bool isOpen() const noexcept { return open; }
private:
    bool open = true;

    void updateColors(Config& config) const noexcept;
    void renderMenuBar(Features& features) noexcept;
    void renderAimbotWindow(Config& config, bool contentOnly = false) noexcept;
    void renderTriggerbotWindow(Config& config, bool contentOnly = false) noexcept;
    void renderChamsWindow(Config& config, bool contentOnly = false) noexcept;
    void renderStyleWindow(Config& config, bool contentOnly = false) noexcept;
    void renderConfigWindow(const OtherInterfaces& interfaces, const Memory& memory, Config& config, bool contentOnly = false) noexcept;
    void renderGuiStyle2(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config) noexcept;

    struct {
        bool aimbot = false;
        bool triggerbot = false;
        bool chams = false;
        bool sound = false;
        bool style = false;
        bool config = false;
    } window;

    struct {
        ImFont* normal15px = nullptr;
    } fonts;

    float timeToNextConfigRefresh = 0.1f;
};

inline std::optional<GUI> gui;
