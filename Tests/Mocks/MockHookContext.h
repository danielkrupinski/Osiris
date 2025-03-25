#pragma once

#include <gmock/gmock.h>

struct MockConfig;
struct MockGameRules;
struct MockHud;
struct MockPanelFactory;

struct MockHookContext {
    MOCK_METHOD(MockHud&, hud, ());
    MOCK_METHOD(MockGameRules&, gameRules, ());
    MOCK_METHOD(MockPanelFactory&, panelFactory, ());
    MOCK_METHOD(MockConfig&, config, ());
};
