#pragma once

#include <functional>
#include <gmock/gmock.h>

struct MockPanel;

struct MockPanelHandle {
    MOCK_METHOD(MockPanel&, getOrInit, (std::function<MockPanel&()>));
    MOCK_METHOD(MockPanel&, get, ());
};
