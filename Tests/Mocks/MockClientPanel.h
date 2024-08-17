#pragma once

#include <gmock/gmock.h>

struct MockPanel;

struct MockClientPanel {
    MOCK_METHOD(MockPanel&, uiPanel, ());
};
