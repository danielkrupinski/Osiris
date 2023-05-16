#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

#include "../JsonForward.h"
#include "../Memory.h"

#include <Platform/Macros/IsPlatform.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include <BytePatterns/ClientPatternFinder.h>

class ClientInterfaces;
class EngineInterfaces;

class Glow {
public:
    Glow(const ClientPatternFinder& clientPatternFinder);

    void render(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const Memory& memory) noexcept;
    void clearCustomObjects() noexcept;
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
    csgo::GlowObjectManager* glowObjectManager;
#if IS_WIN32() || IS_WIN64()
    FunctionInvoker<csgo::GlowObjectAntiCheatCheck> glowObjectAntiCheatCheck;
#endif

    std::unordered_map<std::string, PlayerGlow> playerGlowConfig;
    std::unordered_map<std::string, GlowItem> glowConfig;
    KeyBindToggle glowToggleKey;
    KeyBind glowHoldKey;

    std::vector<std::pair<int, int>> customGlowEntities;
};
