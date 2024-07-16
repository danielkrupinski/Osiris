#pragma once

#include <gmock/gmock.h>

struct MockHud;

struct MockHookContext {
    MOCK_METHOD(MockHud&, hud, ());
};
