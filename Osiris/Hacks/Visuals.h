#pragma once

#include <string_view>

namespace Visuals {
    void reduceFlashEffect() noexcept;
    constexpr bool removeHands(const std::string_view modelName) noexcept
    {
        return config.visuals.noHands && (modelName.find("arms") != std::string_view::npos);
    }

    constexpr bool removeSleeves(const std::string_view modelName) noexcept
    {
        return config.visuals.noSleeves && (modelName.find("sleeve") != std::string_view::npos);
    }
    void modifySmoke() noexcept;
    void thirdperson();
};
