#pragma once

#include <string_view>

#include <Platform/Macros/IsPlatform.h>

namespace cs2
{

#if IS_WIN64()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds\\player\\footsteps\\";
#elif IS_LINUX()
constexpr std::string_view kPlayerFootstepSoundsPath = "sounds/player/footsteps/";
#endif

constexpr std::string_view kPlayerSuitSoundPrefix = "suit";

}
