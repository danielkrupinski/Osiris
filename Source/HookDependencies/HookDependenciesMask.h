#pragma once

#include <Utils/TypeBitFlags.h>

struct FileSystem;
struct SoundChannels;
struct WorldToClipSpaceConverter;

using HookDependenciesMask = TypeBitFlags<
    WorldToClipSpaceConverter,
    SoundChannels,
    FileSystem
>;
