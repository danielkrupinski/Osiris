#pragma once

#include <string_view>

#include <Platform/Macros/IsPlatform.h>

namespace cs2
{

#if IS_WIN64()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds\\player\\footsteps\\";
constexpr std::string_view kBombPlantSoundPath = "sounds\\weapons\\c4\\c4_initiate.vsnd";
#elif IS_LINUX()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds/player/footsteps/";
constexpr std::string_view kBombPlantSoundPath = "sounds/weapons/c4/c4_initiate.vsnd";
#endif

constexpr std::string_view kPlayerSuitSoundPrefix = "suit";

}
