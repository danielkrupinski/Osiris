#pragma once

namespace cs2
{
    struct SoundChannels;
}

struct SoundSystemPatterns {
    [[nodiscard]] static cs2::SoundChannels** soundChannels() noexcept;
};
