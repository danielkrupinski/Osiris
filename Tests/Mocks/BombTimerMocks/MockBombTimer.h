#pragma once

#include <gmock/gmock.h>

#include <Common/Visibility.h>

struct MockBombTimer {
    MOCK_METHOD(Visibility, update, ());
    MOCK_METHOD(void, forceHide, ());
};
