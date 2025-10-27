#pragma once

#include <cstddef>
#include <cstdint>
#include <CS2/Classes/Entities/C_CSPlayerResource.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PlayerResourceOffset = FieldOffset<cs2::C_CSPlayerResource, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(PointerToPlayerResource, cs2::C_CSPlayerResource**);
STRONG_TYPE_ALIAS(OffsetToBombsiteACenter, PlayerResourceOffset<cs2::C_CSPlayerResource::m_bombsiteCenterA, std::int32_t>);
#if IS_WIN64()
STRONG_TYPE_ALIAS(OffsetToBombsiteBCenter, PlayerResourceOffset<cs2::C_CSPlayerResource::m_bombsiteCenterB, std::int32_t>);
#elif IS_LINUX()
STRONG_TYPE_ALIAS(OffsetToBombsiteBCenter, FieldFieldOffset<cs2::C_CSPlayerResource, cs2::C_CSPlayerResource::m_bombsiteCenterB, std::int32_t, offsetof(cs2::C_CSPlayerResource::m_bombsiteCenterB, y)>);
#endif
