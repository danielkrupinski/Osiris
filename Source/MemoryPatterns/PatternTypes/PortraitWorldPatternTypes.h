#pragma once

#include <cstdint>

#include <CS2/Classes/CCS_PortraitWorld.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(OffsetToPortraitWorldEntities, FieldOffset<cs2::CCS_PortraitWorld, cs2::CCS_PortraitWorld::EntityHandles, std::int8_t>);
