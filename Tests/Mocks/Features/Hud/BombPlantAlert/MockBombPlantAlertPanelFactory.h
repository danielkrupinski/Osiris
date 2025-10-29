#pragma once

#include <gmock/gmock.h>

struct MockPanel;

struct MockBombPlantAlertPanelFactory {
    MOCK_METHOD(MockPanel&, createContainerPanel, ());
    MOCK_METHOD(MockPanel&, createBombsiteIconPanel, (MockPanel& parent, const char* iconURL));
    MOCK_METHOD(MockPanel&, createTimerPanel, (MockPanel& parent));
};
