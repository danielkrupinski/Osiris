#pragma once

#include <gmock/gmock.h>

struct MockClientPanel;
struct MockPanel;

struct MockPanelFactory {
    MOCK_METHOD(MockClientPanel&, createPanel, (MockPanel& parentPanel));
    MOCK_METHOD(MockClientPanel&, createLabelPanel, (MockPanel& parentPanel));
};
