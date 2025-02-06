#pragma once

#include <cstdint>

#include <CS2/Panorama/CUI_MapPlayerPreviewPanel.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/StrongTypeAlias.h>

#if IS_WIN64()
#include <Utils/FieldOffset.h>

STRONG_TYPE_ALIAS(OffsetToPlayerPreviewPanelEntities, FieldOffset<cs2::CUI_MapPlayerPreviewPanel, cs2::CUI_MapPlayerPreviewPanel::EntityHandles, std::int8_t>);
#elif IS_LINUX()
#include <cstddef>
#include <Utils/FieldFieldOffset.h>

STRONG_TYPE_ALIAS(OffsetToPlayerPreviewPanelEntities, FieldFieldOffset<cs2::CUI_MapPlayerPreviewPanel, cs2::CUI_MapPlayerPreviewPanel::EntityHandles, std::int8_t, offsetof(cs2::CUI_MapPlayerPreviewPanel::EntityHandles, memory)>);
#endif
