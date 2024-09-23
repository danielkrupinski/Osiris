#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using SmokeGrenadeProjectileOffset = FieldOffset<cs2::C_SmokeGrenadeProjectile, FieldType, OffsetType>;

using OffsetToDidSmokeEffect = SmokeGrenadeProjectileOffset<cs2::C_SmokeGrenadeProjectile::m_bDidSmokeEffect, std::int32_t>;
