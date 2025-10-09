#pragma once

#include <cstdint>

#include <CS2/Panorama/CUI_3dPanel.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(OffsetToPortraitWorld, FieldOffset<cs2::CUI_3dPanel, cs2::CUI_3dPanel::PortraitWorld, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetTo3dPanelFov, FieldOffset<cs2::CUI_3dPanel, cs2::CUI_3dPanel::Fov, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetTo3dPanelFovWeight, FieldOffset<cs2::CUI_3dPanel, cs2::CUI_3dPanel::FovWeight, std::int32_t>);
