#pragma once

#include <string_view>

#include <Platform/Macros/IsPlatform.h>

namespace cs2
{

#if IS_WIN64()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds\\player\\footsteps\\";
constexpr std::string_view kBombPlantSoundPath = "sounds\\weapons\\c4\\c4_initiate.vsnd";
constexpr std::string_view kBombSoundsPath = "sounds\\weapons\\c4\\";
#elif IS_LINUX()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds/player/footsteps/";
constexpr std::string_view kBombPlantSoundPath = "sounds/weapons/c4/c4_initiate.vsnd";
constexpr std::string_view kBombSoundsPath = "sounds/weapons/c4/";
#endif

constexpr std::string_view kPlayerSuitSoundPrefix = "suit";
constexpr std::string_view kBombBeepSoundsPrefix = "c4_beep";

}
