#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

#include "../JsonForward.h"
#include "../Memory.h"

#include <Platform/Macros/IsPlatform.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>

class ClientInterfaces;
class EngineInterfaces;

class Glow {
public:
    Glow(const PatternFinder& clientPatternFinder)
        :
#if IS_WIN32()
        glowObjectManager{ clientPatternFinder("0F 11 05 ? ? ? ? 83 C8 01"_pat).add(3).deref().as<csgo::GlowObjectManager*>() },
        glowObjectAntiCheatCheck{ retSpoofGadgets->client, clientPatternFinder("E8 ? ? ? ? 8B 75 FC 6A 04"_pat).add(1).relativeToAbsolute().get() }
#elif IS_LINUX()
        glowObjectManager{ clientPatternFinder("E8 ? ? ? ? 4C 89 E7 8B 70 20"_pat).add(1).relativeToAbsolute().add(12).relativeToAbsolute().as<csgo::GlowObjectManager*>() }
#endif
    {
    }
 
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
#if IS_WIN32()
    FunctionInvoker<csgo::GlowObjectAntiCheatCheck> glowObjectAntiCheatCheck;
#endif

    std::unordered_map<std::string, PlayerGlow> playerGlowConfig;
    std::unordered_map<std::string, GlowItem> glowConfig;
    KeyBindToggle glowToggleKey;
    KeyBind glowHoldKey;

    std::vector<std::pair<int, int>> customGlowEntities;
};
