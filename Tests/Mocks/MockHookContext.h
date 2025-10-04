#pragma once

#include <gmock/gmock.h>
#include <Features/FeaturesStates.h>

struct ConfigState;
struct MockConfig;
struct MockGameRules;
struct MockGui;
struct MockHud;
struct MockPanelFactory;

class OsirisDirectoryPath;

struct MockHookContext {
    MOCK_METHOD(MockHud&, hud, ());
    MOCK_METHOD(MockGameRules&, gameRules, ());
    MOCK_METHOD(MockPanelFactory&, panelFactory, ());
    MOCK_METHOD(MockConfig&, config, ());
    MOCK_METHOD(ConfigState&, configState, ());
    MOCK_METHOD(OsirisDirectoryPath&, osirisDirectoryPath, ());
    MOCK_METHOD(MockGui&, gui, ());
    MOCK_METHOD(FeaturesStates&, featuresStates, ());
};
