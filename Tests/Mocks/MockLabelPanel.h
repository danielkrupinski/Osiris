#pragma once

#include <gmock/gmock.h>

struct MockPanel;

struct MockLabelPanel {
    MOCK_METHOD(void, setText, (const char* text));
    MOCK_METHOD(MockPanel&, uiPanel, ());
};
