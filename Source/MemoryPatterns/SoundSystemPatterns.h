#pragma once

namespace cs2
{
    struct SoundChannels;
}

struct SoundSystemPatterns {
    [[nodiscard]] cs2::SoundChannels** soundChannels() const noexcept;
};
