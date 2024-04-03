#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PlayerControllerOffset = FieldOffset<cs2::CCSPlayerController, FieldType, OffsetType>;

using OffsetToPlayerPawnHandle = PlayerControllerOffset<cs2::CCSPlayerController::m_hPawn, std::int32_t>;
