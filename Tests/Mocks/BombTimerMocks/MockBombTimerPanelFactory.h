#pragma once

#include <gmock/gmock.h>

struct MockPanel;

struct MockBombTimerPanelFactory {
    MOCK_METHOD(MockPanel&, createContainerPanel, (MockPanel& parentPanel));
    MOCK_METHOD(MockPanel&, createBombsiteIconPanel, (MockPanel& parentPanel));
    MOCK_METHOD(MockPanel&, createTimerTextPanel, (MockPanel& parentPanel));
};
