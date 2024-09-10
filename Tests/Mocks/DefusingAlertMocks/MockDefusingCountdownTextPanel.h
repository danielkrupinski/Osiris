#pragma once

#include <gmock/gmock.h>
#include <Utils/Optional.h>

struct MockDefusingCountdownTextPanel {
    MOCK_METHOD(MockDefusingCountdownTextPanel&, setTimeToDefuseEnd, (Optional<float> timeToDefuseEnd));
    MOCK_METHOD(MockDefusingCountdownTextPanel&, setCanBeDefused, (Optional<bool> canBeDefused));
};
