#pragma once

#include <cstdint>

#include <CS2/Classes/CCSPlayer_HostageServices.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using HostageServicesOffset = FieldOffset<cs2::CCSPlayer_HostageServices, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToCarriedHostage, HostageServicesOffset<cs2::CCSPlayer_HostageServices::m_hCarriedHostage, std::int8_t>);
