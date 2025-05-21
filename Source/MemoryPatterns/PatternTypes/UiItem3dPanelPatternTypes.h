#pragma once

#include <cstdint>

#include <CS2/Panorama/CUI_Item3dPanel.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(SetItemItemIdFunction, cs2::CUI_Item3dPanel::SetItemItemId*);
STRONG_TYPE_ALIAS(OffsetToItem3dPanelUnknownField, FieldOffset<cs2::CUI_Item3dPanel, cs2::CUI_Item3dPanel::UnknownField, std::int32_t>);
