#pragma once

#include <string_view>

#include <Helpers/ConstString.h>
#include "LinuxPlatformApi.h"

struct UserHomeFolderPath {
    [[nodiscard]] std::string_view get() const noexcept
    {
        return homePath.get();
    }

private:
    [[nodiscard]] static ConstString getHomePath() noexcept
    {
        if (const auto home = LinuxPlatformApi::getenv("HOME"))
            return ConstString{ home };
        return {};
    }

    ConstString homePath = getHomePath();
};
