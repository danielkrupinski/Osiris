#pragma once

#include <gmock/gmock.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <Utils/Optional.h>

struct PanelFontParams;

struct MockPanel {
    MOCK_METHOD(void, setParent, (const MockPanel& parent));
    MOCK_METHOD(void, show, ());
    MOCK_METHOD(void, hide, ());
    MOCK_METHOD(Optional<bool>, isVisible, ());
    MOCK_METHOD(void, setWidth, (cs2::CUILength width));
    MOCK_METHOD(void, setHeight, (cs2::CUILength width));
    MOCK_METHOD(void, setMixBlendMode, (cs2::EMixBlendMode mode));
    MOCK_METHOD(void, setTextAlign, (cs2::ETextAlign textAlign));
    MOCK_METHOD(void, setFont, (const PanelFontParams& params));
};
