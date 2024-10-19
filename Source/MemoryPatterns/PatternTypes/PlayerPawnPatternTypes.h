#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using PlayerPawnOffset = FieldOffset<cs2::C_CSPlayerPawn, FieldType, OffsetType>;

STRONG_TYPE(OffsetToPlayerPawnImmunity, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bGunGameImmunity, std::int32_t>);
STRONG_TYPE(OffsetToWeaponServices, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_pWeaponServices, std::int32_t>);
STRONG_TYPE(OffsetToPlayerController, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_hController, std::int32_t>);
STRONG_TYPE(OffsetToIsDefusing, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsDefusing, std::int32_t>);
STRONG_TYPE(OffsetToIsPickingUpHostage, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsGrabbingHostage, std::int32_t>);
STRONG_TYPE(OffsetToHostageServices, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_pHostageServices, std::int32_t>);
STRONG_TYPE(OffsetToFlashBangEndTime, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_flFlashBangTime, std::int32_t>);
