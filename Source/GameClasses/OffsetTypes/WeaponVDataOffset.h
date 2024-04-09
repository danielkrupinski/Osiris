#pragma once

#include <cstdint>

#include <CS2/Classes/CCSWeaponBaseVData.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using WeaponVDataOffset = FieldOffset<cs2::CCSWeaponBaseVData, FieldType, OffsetType>;

using OffsetToWeaponName = WeaponVDataOffset<cs2::CCSWeaponBaseVData::m_szName, std::int32_t>;
