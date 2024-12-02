#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using WeaponOffset = FieldOffset<cs2::C_CSWeaponBase, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToClipAmmo, WeaponOffset<cs2::C_CSWeaponBase::m_iClip1, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToWeaponSceneObjectUpdaterHandle, WeaponOffset<cs2::C_CSWeaponBase::sceneObjectUpdaterHandle, std::int32_t>);
