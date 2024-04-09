#pragma once

#include <cstdint>

#include <CS2/Classes/CCSPlayer_WeaponServices.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using WeaponServicesOffset = FieldOffset<cs2::CCSPlayer_WeaponServices, FieldType, OffsetType>;

using OffsetToActiveWeapon = WeaponServicesOffset<cs2::CCSPlayer_WeaponServices::m_hActiveWeapon, std::int8_t>;
