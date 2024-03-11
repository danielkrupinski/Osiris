#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using EntityOffset = FieldOffset<cs2::C_BaseEntity, FieldType, OffsetType>;
