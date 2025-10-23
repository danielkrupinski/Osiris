#pragma once

#include <gmock/gmock.h>

#include <CS2/Classes/Color.h>
#include <Utils/Optional.h>

struct MockCrosshair {
    MOCK_METHOD(Optional<cs2::Color>, getColor, ());
};
