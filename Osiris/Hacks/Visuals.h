#pragma once

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
        return config.visuals.noHands && strstr(modelName, "arms") && !strstr(modelName, "sleeve");
    }

    constexpr bool removeSleeves(const char* modelName) noexcept
    {
        return config.visuals.noSleeves && strstr(modelName, "sleeve");
    }

    constexpr bool removeWeapons(const char* modelName) noexcept
    {
        return config.visuals.noWeapons && strstr(modelName, "models/weapons/v_")
            && !strstr(modelName, "arms") && !strstr(modelName, "tablet")
            && !strstr(modelName, "parachute") && !strstr(modelName, "fists");
    }

    constexpr void skybox() noexcept
    {
        constexpr const char* skyboxes[]{ "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam" };

        if (config.visuals.skybox)
            memory.loadSky(skyboxes[config.visuals.skybox - 1]);
        else
            memory.loadSky(interfaces.cvar->findVar("sv_skyname")->string);
    }
};
