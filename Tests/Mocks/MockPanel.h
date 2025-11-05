#pragma once

#include <gmock/gmock.h>
#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/PanelHandle.h>
#include <CS2/Panorama/StyleEnums.h>
#include <Utils/Optional.h>

struct MockClientPanel;
struct PanelFontParams;
struct PanelAlignmentParams;
struct PanelMarginParams;

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
    MOCK_METHOD(void, setVisible, (bool visible));
    MOCK_METHOD(void, setBorderRadius, (cs2::CUILength radius));
    MOCK_METHOD(void, setAlign, (const PanelAlignmentParams& alignmentParams));
    MOCK_METHOD(void, setBackgroundColor, (cs2::Color color));
    MOCK_METHOD(void, setBorder, (cs2::CUILength borderWidth, cs2::Color color));
    MOCK_METHOD(void, setColor, (cs2::Color color));
    MOCK_METHOD(void, setFlowChildren, (cs2::EFlowDirection flowDirection));
    MOCK_METHOD(void, setPosition, (cs2::CUILength x, cs2::CUILength y));
    MOCK_METHOD(void, setMargin, (const PanelMarginParams& params));
    MOCK_METHOD(cs2::PanelHandle, getHandle, ());
    MOCK_METHOD(MockClientPanel&, clientPanel, ());
};
