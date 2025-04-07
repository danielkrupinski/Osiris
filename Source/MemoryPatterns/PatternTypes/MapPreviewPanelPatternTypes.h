#pragma once

#include <cstdint>

#include <CS2/Panorama/CUI_MapPreviewPanel.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/StrongTypeAlias.h>

#if IS_WIN64()
#include <Utils/FieldOffset.h>

STRONG_TYPE_ALIAS(OffsetToMapPreviewPanelEntities, FieldOffset<cs2::CUI_MapPreviewPanel, cs2::CUI_MapPreviewPanel::EntityHandles, std::int8_t>);
#elif IS_LINUX()
#include <cstddef>
#include <Utils/FieldFieldOffset.h>

STRONG_TYPE_ALIAS(OffsetToMapPreviewPanelEntities, FieldFieldOffset<cs2::CUI_MapPreviewPanel, cs2::CUI_MapPreviewPanel::EntityHandles, std::int8_t, offsetof(cs2::CUI_MapPreviewPanel::EntityHandles, memory)>);
#endif
