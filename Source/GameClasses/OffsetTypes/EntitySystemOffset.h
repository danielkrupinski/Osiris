#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityIndex.h>
#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using EntitySystemOffset = FieldOffset<cs2::CGameEntitySystem, FieldType, OffsetType>;

using HighestEntityIndexOffset = EntitySystemOffset<cs2::CEntityIndex, std::int32_t>;
using EntityListOffset = EntitySystemOffset<cs2::CConcreteEntityList, std::int8_t>;
