#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PlayerControllerOffset = FieldOffset<cs2::CCSPlayerController, FieldType, OffsetType>;
