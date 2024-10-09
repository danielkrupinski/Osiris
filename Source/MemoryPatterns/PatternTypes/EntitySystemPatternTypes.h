#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityIndex.h>
#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using EntitySystemOffset = FieldOffset<cs2::CGameEntitySystem, FieldType, OffsetType>;

STRONG_TYPE(EntitySystemPointer, cs2::CGameEntitySystem**);
STRONG_TYPE(HighestEntityIndexOffset, EntitySystemOffset<cs2::CEntityIndex, std::int32_t>);
STRONG_TYPE(EntityListOffset, EntitySystemOffset<cs2::CConcreteEntityList, std::int8_t>);
