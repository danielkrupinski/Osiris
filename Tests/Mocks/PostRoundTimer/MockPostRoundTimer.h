#pragma once

#include <gmock/gmock.h>

#include <Common/Visibility.h>

struct MockPostRoundTimer {
    MOCK_METHOD(Visibility, update, ());
};
