#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PlayerPawnOffset = FieldOffset<cs2::C_CSPlayerPawn, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToPlayerPawnImmunity, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bGunGameImmunity, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToWeaponServices, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_pWeaponServices, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToPlayerController, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_hController, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToIsDefusing, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsDefusing, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToIsPickingUpHostage, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsGrabbingHostage, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToHostageServices, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_pHostageServices, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToFlashBangEndTime, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_flFlashBangTime, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToPlayerPawnSceneObjectUpdaterHandle, PlayerPawnOffset<cs2::C_CSPlayerPawn::sceneObjectUpdaterHandle, std::int32_t>);
