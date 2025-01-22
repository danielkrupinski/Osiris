#pragma once

#include <array>

#include <Utils/TypeBitFlags.h>
#include "WatchedSounds.h"

template <typename... Sounds>
struct SoundWatcherImplState {
    TypeBitFlags<Sounds...> soundsToWatch;
    std::array<WatchedSounds, sizeof...(Sounds)> watchedSounds;
};
