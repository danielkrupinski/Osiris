#pragma once

#include <gmock/gmock.h>

#include <Common/Visibility.h>

struct MockBombStatusPanelContext {
    MOCK_METHOD(MockPanel&, bombStatusPanel, ());
    MOCK_METHOD(MockPanel&, scoreAndTimeAndBombPanel, ());
    MOCK_METHOD(MockPanel&, invisiblePanel, ());
    MOCK_METHOD(Visibility&, visibility, ());
};
