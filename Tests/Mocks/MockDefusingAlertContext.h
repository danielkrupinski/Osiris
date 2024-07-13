#pragma once

struct MockDefusingCountdownTextPanel;
struct MockPanel;

struct MockDefusingAlertContext {
    MOCK_METHOD(MockPanel&, defusingAlertContainerPanel, ());
    MOCK_METHOD(MockDefusingCountdownTextPanel&, defusingCountdownTextPanel, ());
};
