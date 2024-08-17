#pragma once

#include <gmock/gmock.h>

struct MockBombStatusPanel {
    MOCK_METHOD(void, hide, ());
    MOCK_METHOD(void, restore, ());
};
