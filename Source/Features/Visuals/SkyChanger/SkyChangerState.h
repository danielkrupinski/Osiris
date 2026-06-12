#pragma once

#include <cstdint>
#include <string_view>

enum class SkyType : std::uint8_t {
    Default = 0,
    Mirage,
    Inferno,
    Nuke,
    Dust2,
    Vertigo,
    Ancient,
    Anubis,
    Office,
    Italy,
    COUNT
};

struct SkyChangerState {
    SkyType currentSky = SkyType::Default;
    bool enabled = false;
};
