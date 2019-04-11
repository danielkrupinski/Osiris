#pragma once

#include <string_view>

#include "../Memory.h"

enum class FrameStage {
    UNDEFINED = -1,
    START,
    NET_UPDATE_START,
    NET_UPDATE_POSTDATAUPDATE_START,
    NET_UPDATE_POSTDATAUPDATE_END,
    NET_UPDATE_END,
    RENDER_START,
    RENDER_END
};

namespace Visuals {
    void colorWorld() noexcept;
    void modifySmoke() noexcept;
    void thirdperson() noexcept;
    void removeVisualRecoil(FrameStage) noexcept;
    void removeBlur() noexcept;
    void updateBrightness() noexcept;
    void removeGrass() noexcept;
    void remove3dSky() noexcept;

    constexpr void disablePostProcessing() noexcept
    {
        if (*memory.disablePostProcessing != config.visuals.disablePostProcessing)
            *memory.disablePostProcessing = config.visuals.disablePostProcessing;
    }

    constexpr void reduceFlashEffect() noexcept
    {
        interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->setProperty("m_flFlashMaxAlpha", 255.0f - config.visuals.flashReduction * 2.55f);
    }

    constexpr bool removeHands(const char* modelName) noexcept
    {
        return config.visuals.noHands && strstr(modelName, "arms");
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

        if (config.visuals.skybox != -1)
            memory.loadSky(skyboxes[config.visuals.skybox]);
    }
};
