#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PlayerControllerOffset = FieldOffset<cs2::CCSPlayerController, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToPlayerPawnHandle, PlayerControllerOffset<cs2::CCSPlayerController::m_hPlayerPawn, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToPlayerColor, PlayerControllerOffset<cs2::CCSPlayerController::m_iCompTeammateColor, std::int32_t>);
