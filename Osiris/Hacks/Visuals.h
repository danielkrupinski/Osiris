#pragma once

#include <string_view>

namespace Visuals {
    bool removeHands(const std::string_view) noexcept;
    bool removeSleeves(const std::string_view) noexcept;
    void thirdperson();
};
