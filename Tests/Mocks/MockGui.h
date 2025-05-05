#pragma once

#include <gmock/gmock.h>

struct MockGui {
    MOCK_METHOD(void, updateFromConfig, ());
};
