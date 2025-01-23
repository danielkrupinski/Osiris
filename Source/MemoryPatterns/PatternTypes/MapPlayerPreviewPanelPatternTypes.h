#pragma once

#include <cstdint>

#include <CS2/Panorama/CUI_MapPlayerPreviewPanel.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/FieldFieldOffset.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

#if IS_WIN64()
STRONG_TYPE_ALIAS(OffsetToPlayerPreviewPanelEntities, FieldOffset<cs2::CUI_MapPlayerPreviewPanel, cs2::CUI_MapPlayerPreviewPanel::EntityHandles, std::int8_t>);
#elif IS_LINUX()
STRONG_TYPE_ALIAS(OffsetToPlayerPreviewPanelEntities, FieldFieldOffset<cs2::CUI_MapPlayerPreviewPanel, cs2::CUI_MapPlayerPreviewPanel::EntityHandles, std::int8_t, offsetof(cs2::CUI_MapPlayerPreviewPanel::EntityHandles, memory)>);
#endif
