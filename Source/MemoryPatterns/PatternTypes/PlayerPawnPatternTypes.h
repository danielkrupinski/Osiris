#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PlayerPawnOffset = FieldOffset<cs2::C_CSPlayerPawn, FieldType, OffsetType>;

template <typename FieldType, typename OffsetType>
using PlayerMovementServicesOffset = FieldOffset<cs2::CCSPlayer_MovementServices, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToPlayerPawnImmunity, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bGunGameImmunity, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToMovementServices, PlayerPawnOffset<cs2::C_BasePlayerPawn::m_pMovementServices, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToWeaponServices, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_pWeaponServices, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToPlayerController, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_hController, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToIsDefusing, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsDefusing, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToIsPickingUpHostage, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsGrabbingHostage, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToHostageServices, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_pHostageServices, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToFlashBangEndTime, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_flFlashBangTime, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToPlayerPawnSceneObjectUpdaterHandle, PlayerPawnOffset<cs2::C_CSPlayerPawn::sceneObjectUpdaterHandle, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToIsScoped, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_bIsScoped, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToEyeAngles, PlayerPawnOffset<cs2::C_CSPlayerPawn::m_angEyeAngles, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToStashGrenadeParameterWhen, PlayerMovementServicesOffset<cs2::CCSPlayer_MovementServices::m_fStashGrenadeParameterWhen, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToGrenadeParametersStashed, PlayerMovementServicesOffset<cs2::CCSPlayer_MovementServices::m_bGrenadeParametersStashed, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToStashedShootAngles, PlayerMovementServicesOffset<cs2::CCSPlayer_MovementServices::m_angStashedShootAngles, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToStashedGrenadeThrowPosition, PlayerMovementServicesOffset<cs2::CCSPlayer_MovementServices::m_vecStashedGrenadeThrowPosition, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToStashedVelocity, PlayerMovementServicesOffset<cs2::CCSPlayer_MovementServices::m_vecStashedVelocity, std::int32_t>);
