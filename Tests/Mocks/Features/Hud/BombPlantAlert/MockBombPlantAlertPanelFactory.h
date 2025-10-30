#pragma once

#include <gmock/gmock.h>

struct MockPanel;

struct MockBombPlantAlertPanelFactory {
    MOCK_METHOD(MockPanel&, createContainerPanel, (MockPanel& parent));
    MOCK_METHOD(MockPanel&, createBombsiteIconPanel, (MockPanel& container, const char* iconURL));
    MOCK_METHOD(MockPanel&, createTimerPanel, (MockPanel& container));
};
