#pragma once

#include <optional>
#include <gmock/gmock.h>

struct MockViewToProjectionMatrix {
    MOCK_METHOD(std::optional<float>, transformY, (float yInViewSpace));
};
