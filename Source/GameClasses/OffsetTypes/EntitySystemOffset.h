#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using EntitySystemOffset = FieldOffset<cs2::CGameEntitySystem, FieldType, OffsetType>;
