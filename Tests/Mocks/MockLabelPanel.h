#pragma once

#include <gmock/gmock.h>

#include <CS2/Classes/Color.h>

struct MockUiPanel {
    MOCK_METHOD(void, setColor, (cs2::Color color));
};

struct MockLabelPanel {
    MOCK_METHOD(void, setText, (const char* text));
    MOCK_METHOD(MockUiPanel&, uiPanel, ());
};
