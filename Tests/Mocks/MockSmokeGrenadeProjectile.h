#pragma once

#include <gmock/gmock.h>
#include <Utils/Optional.h>

struct MockSmokeGrenadeProjectile {
    MOCK_METHOD(Optional<bool>, didSmokeEffect, ());
};
