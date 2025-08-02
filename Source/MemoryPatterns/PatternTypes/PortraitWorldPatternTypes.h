#pragma once

#include <cstdint>

#include <CS2/Classes/CCS_PortraitWorld.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(OffsetToPortraitWorldEntities, FieldOffset<cs2::CCS_PortraitWorld, cs2::CCS_PortraitWorld::DynamicEntityHandles, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToPortraitWorldMapEntities, FieldOffset<cs2::CCS_PortraitWorld, cs2::CCS_PortraitWorld::MapEntityHandles, std::int32_t>);
