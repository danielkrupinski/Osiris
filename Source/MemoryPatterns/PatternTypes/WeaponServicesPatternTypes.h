#pragma once

#include <cstdint>

#include <CS2/Classes/CCSPlayer_WeaponServices.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using WeaponServicesOffset = FieldOffset<cs2::CCSPlayer_WeaponServices, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToActiveWeapon, WeaponServicesOffset<cs2::CCSPlayer_WeaponServices::m_hActiveWeapon, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToWeapons, WeaponServicesOffset<cs2::CCSPlayer_WeaponServices::m_hMyWeapons, std::int8_t>);
