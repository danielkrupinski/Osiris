#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityIndex.h>
#include <CS2/Classes/EntitySystem/CEntityClass.h>
#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <CS2/Classes/CUtlString.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/FieldFieldOffset.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using EntitySystemOffset = FieldOffset<cs2::CGameEntitySystem, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(EntitySystemPointer, cs2::CGameEntitySystem**);
STRONG_TYPE_ALIAS(HighestEntityIndexOffset, EntitySystemOffset<cs2::CEntityIndex, std::int32_t>);
STRONG_TYPE_ALIAS(EntityListOffset, EntitySystemOffset<cs2::CConcreteEntityList, std::int8_t>);

#if defined(_WIN64)
STRONG_TYPE_ALIAS(FindEntityClassFunctionPointer, cs2::CEntityClass*(*)(cs2::CGameEntitySystem*, const char*, cs2::CUtlString*) noexcept);
STRONG_TYPE_ALIAS(EntityClassParentNodeOffset, FieldOffset<cs2::CEntityClass, cs2::CEntityClassParentNode*, std::uint8_t>);
STRONG_TYPE_ALIAS(EntityClassParentOuterOffset, FieldOffset<cs2::CEntityClassParentNode, cs2::CEntityClass*, std::uint8_t>);
STRONG_TYPE_ALIAS(EntityClassParentNodeOffsetConsistency, std::uint8_t);
#else
// Entity-class registration ancestry is Windows-only until the Linux descriptor ABI is available.
STRONG_TYPE_ALIAS(OffsetToEntityClasses, FieldFieldOffset<cs2::CGameEntitySystem, cs2::CGameEntitySystem::EntityClasses, std::int32_t, offsetof(cs2::CGameEntitySystem::EntityClasses, memory)>);
#endif
