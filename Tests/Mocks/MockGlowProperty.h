#pragma once

#include <gmock/gmock.h>
#include <Utils/Optional.h>

struct MockGlowProperty {
    MOCK_METHOD(Optional<bool>, isGlowing, ());
};
