#pragma once

#include <string_view>

namespace Visuals {
    void colorWorld() noexcept;
    void reduceFlashEffect() noexcept;
    void modifySmoke() noexcept;
    void thirdperson() noexcept;

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
};
