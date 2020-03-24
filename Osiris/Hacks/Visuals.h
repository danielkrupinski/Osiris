#pragma once

#include <array>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/Engine.h"
#include "../SDK/EntityList.h"

enum class FrameStage;
class GameEvent;

namespace Visuals {
    void playerModel(FrameStage stage) noexcept;
    void colorWorld() noexcept;
    void modifySmoke() noexcept;
    void thirdperson() noexcept;
    void removeVisualRecoil(FrameStage) noexcept;
    void removeBlur() noexcept;
    void updateBrightness() noexcept;
    void removeGrass() noexcept;
    void remove3dSky() noexcept;
    void removeShadows() noexcept;
    void applyZoom(FrameStage) noexcept;
    void applyScreenEffects() noexcept;
    void hitEffect(GameEvent* = nullptr) noexcept;
    void hitMarker(GameEvent* = nullptr) noexcept;

    constexpr void disablePostProcessing() noexcept
    {
        if (*memory.disablePostProcessing != config.visuals.disablePostProcessing)
            *memory.disablePostProcessing = config.visuals.disablePostProcessing;
    }

    constexpr auto reduceFlashEffect = []() noexcept
    {
        interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->flashMaxAlpha() = 255.0f - config.visuals.flashReduction * 2.55f;
    };

    constexpr bool removeHands(const char* modelName) noexcept
    {
        return config.visuals.noHands && std::strstr(modelName, "arms") && !std::strstr(modelName, "sleeve");
    }

    constexpr bool removeSleeves(const char* modelName) noexcept
    {
        return config.visuals.noSleeves && std::strstr(modelName, "sleeve");
    }

    constexpr bool removeWeapons(const char* modelName) noexcept
    {
        return config.visuals.noWeapons && std::strstr(modelName, "models/weapons/v_")
            && !std::strstr(modelName, "arms") && !std::strstr(modelName, "tablet")
            && !std::strstr(modelName, "parachute") && !std::strstr(modelName, "fists");
    }

    constexpr void skybox() noexcept
    {
        constexpr std::array skyboxes{ "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam" };

        if (static_cast<std::size_t>(config.visuals.skybox - 1) < skyboxes.size())
            memory.loadSky(skyboxes[config.visuals.skybox - 1]);
        else
            memory.loadSky(interfaces.cvar->findVar("sv_skyname")->string);
    }
};
