#pragma once

#include <gmock/gmock.h>

struct MockClientPanel;
struct MockImagePanel;
struct MockPanel;

struct MockPanelFactory {
    MOCK_METHOD(MockClientPanel&, createPanel, (MockPanel& parentPanel));
    MOCK_METHOD(MockClientPanel&, createPanel, (MockPanel& parentPanel, const char* id));
    MOCK_METHOD(MockClientPanel&, createLabelPanel, (MockPanel& parentPanel));
    MOCK_METHOD(MockImagePanel&, createImagePanel, (MockPanel& parentPanel));
};
