#pragma once

struct DefusingAlertState;
struct MockDefusingAlertCondition;
struct MockDefusingAlertPanel;
struct MockDefusingCountdownTextPanel;
struct MockPanel;
struct MockPlantedC4;

struct MockDefusingAlertContext {
    MOCK_METHOD(MockPanel&, defusingAlertContainerPanel, ());
    MOCK_METHOD(MockDefusingCountdownTextPanel&, defusingCountdownTextPanel, ());
    MOCK_METHOD(MockPlantedC4&, c4BeingDefused, ());
    MOCK_METHOD(bool, hasC4BeingDefused, ());
    MOCK_METHOD(MockDefusingAlertCondition&, defusingAlertCondition, ());
    MOCK_METHOD(MockDefusingAlertPanel&, defusingAlertPanel, ());
    MOCK_METHOD(DefusingAlertState&, state, ());
};
