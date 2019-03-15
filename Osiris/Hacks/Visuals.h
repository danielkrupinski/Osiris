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
    void reduceFlashEffect() noexcept;
    void modifySmoke() noexcept;
    void thirdperson() noexcept;
    void removeVisualRecoil(FrameStage) noexcept;
    void removeBlur() noexcept;
    void updateBrightness() noexcept;
    void removeGrass() noexcept;

    constexpr void disablePostProcessing() noexcept
    {
        if (*memory.disablePostProcessing != config.visuals.disablePostProcessing)
            *memory.disablePostProcessing = config.visuals.disablePostProcessing;
    }

    constexpr bool removeHands(const std::string_view modelName) noexcept
    {
        return config.visuals.noHands && (modelName.find("arms") != std::string_view::npos);
    }

    constexpr bool removeSleeves(const std::string_view modelName) noexcept
    {
        return config.visuals.noSleeves && (modelName.find("sleeve") != std::string_view::npos);
    }

    constexpr bool removeWeapons(const std::string_view modelName) noexcept
    {
        return config.visuals.noWeapons && modelName.find("models/weapons/v_") != std::string_view::npos
            && modelName.find("arms") == std::string_view::npos
            && modelName.find("tablet") == std::string_view::npos
            && modelName.find("parachute") == std::string_view::npos
            && modelName.find("fists") == std::string_view::npos;
    }

    constexpr void skybox() noexcept
    {
        constexpr const char* skyboxes[]{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam" };

        if (config.visuals.skybox)
            reinterpret_cast<void(__fastcall*)(const char*)>(memory.loadSky)(skyboxes[config.visuals.skybox]);
    }
};
