#pragma once

#include <Utils/TypeBitFlags.h>

struct SoundChannels;
struct WorldToClipSpaceConverter;

using HookDependenciesMask = TypeBitFlags<
    WorldToClipSpaceConverter,
    SoundChannels
>;
