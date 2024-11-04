#pragma once

#include <string_view>

#include <Platform/Macros/IsPlatform.h>

namespace cs2
{

#if IS_WIN64()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds\\player\\footsteps\\";
constexpr std::string_view kBombPlantSoundPath = "sounds\\weapons\\c4\\c4_initiate.vsnd";
constexpr std::string_view kBombSoundsPath = "sounds\\weapons\\c4\\";
constexpr std::string_view kWeaponSoundsPath = "sounds\\weapons\\";
#elif IS_LINUX()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds/player/footsteps/";
constexpr std::string_view kBombPlantSoundPath = "sounds/weapons/c4/c4_initiate.vsnd";
constexpr std::string_view kBombSoundsPath = "sounds/weapons/c4/";
constexpr std::string_view kWeaponSoundsPath = "sounds/weapons/";
#endif

constexpr std::string_view kZoomSoundNamePart = "zoom";
constexpr std::string_view kPlayerSuitSoundPrefix = "suit";
constexpr std::string_view kBassSoundPrefix = "bass";
constexpr std::string_view kBombBeepSoundsPrefix = "c4_beep";
constexpr std::string_view kBombDefuseStartSoundName = "c4_disarmstart.vsnd";
constexpr std::string_view kClipOutSoundSuffix = "clipout.vsnd";
constexpr std::string_view kCoverUpSoundSuffix = "coverup.vsnd";

}
