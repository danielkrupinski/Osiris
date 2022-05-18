#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ConfigStructs.h"
#include "InputUtil.h"

struct ImFont;

class Config {
public:
    Config() noexcept;
    void load(std::size_t, bool incremental) noexcept;
    void load(const char8_t* name, bool incremental) noexcept;
    void save(std::size_t) const noexcept;
    void add(const char8_t*) noexcept;
    void remove(std::size_t) noexcept;
    void rename(std::size_t, std::u8string_view newName) noexcept;
    void reset() noexcept;
    void listConfigs() noexcept;
    void createConfigDir() const noexcept;
    void openConfigDir() const noexcept;

    constexpr auto& getConfigs() noexcept
    {
        return configs;
    }

    struct Aimbot {
        bool enabled{ false };
        bool aimlock{ false };
        bool silent{ false };
        bool friendlyFire{ false };
        bool visibleOnly{ true };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool autoScope{ false };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
        int minDamage{ 1 };
        bool killshot{ false };
        bool betweenShots{ true };
    };
    std::array<Aimbot, 40> aimbot;
    bool aimbotOnKey{ false };
    KeyBind aimbotKey;
    int aimbotKeyMode{ 0 };

    struct Triggerbot {
        bool enabled = false;
        bool friendlyFire = false;
        bool scopedOnly = true;
        bool ignoreFlash = false;
        bool ignoreSmoke = false;
        bool killshot = false;
        int hitgroup = 0;
        int shotDelay = 0;
        int minDamage = 1;
        float burstTime = 0.0f;
    };
    std::array<Triggerbot, 40> triggerbot;
    KeyBind triggerbotHoldKey;

    struct Chams {
        struct Material : Color4 {
            bool enabled = false;
            bool healthBased = false;
            bool blinking = false;
            bool wireframe = false;
            bool cover = false;
            bool ignorez = false;
            int material = 0;
        };
        std::array<Material, 7> materials;
    };

    std::unordered_map<std::string, Chams> chams;
    KeyBindToggle chamsToggleKey;
    KeyBind chamsHoldKey;

    struct StreamProofESP {
        KeyBindToggle toggleKey;
        KeyBind holdKey;

        std::unordered_map<std::string, Player> allies;
        std::unordered_map<std::string, Player> enemies;
        std::unordered_map<std::string, Weapon> weapons;
        std::unordered_map<std::string, Projectile> projectiles;
        std::unordered_map<std::string, Shared> lootCrates;
        std::unordered_map<std::string, Shared> otherEntities;
    } streamProofESP;

    struct Font {
        ImFont* tiny;
        ImFont* medium;
        ImFont* big;
    };
   
    struct Style {
        int menuStyle{ 0 };
        int menuColors{ 0 };
    } style;

    void scheduleFontLoad(const std::string& name) noexcept;
    bool loadScheduledFonts() noexcept;
    const auto& getSystemFonts() const noexcept { return systemFonts; }
    const auto& getFonts() const noexcept { return fonts; }
private:
    std::vector<std::string> scheduledFonts{ "Default" };
    std::vector<std::string> systemFonts{ "Default" };
    std::unordered_map<std::string, Font> fonts;
    std::filesystem::path path;
    std::vector<std::u8string> configs;
};

inline std::unique_ptr<Config> config;
