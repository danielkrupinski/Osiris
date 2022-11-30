#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

#include "../JsonForward.h"
#include "../Memory.h"

class ClientInterfaces;
class EngineInterfaces;

class Glow {
public:
    void render(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept;
    void clearCustomObjects(const Memory& memory) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;

    struct GlowItem : Color4 {
        bool enabled = false;
        bool healthBased = false;
        int style = 0;
    };

    struct PlayerGlow {
        GlowItem all, visible, occluded;
    };

private:

    std::unordered_map<std::string, PlayerGlow> playerGlowConfig;
    std::unordered_map<std::string, GlowItem> glowConfig;
    KeyBindToggle glowToggleKey;
    KeyBind glowHoldKey;

    std::vector<std::pair<int, int>> customGlowEntities;
};
