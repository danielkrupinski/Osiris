#pragma once

#include <optional>

#include <gmock/gmock.h>

struct MockDefusingCountdownTextPanel {
    MOCK_METHOD(MockDefusingCountdownTextPanel&, setTimeToDefuseEnd, (std::optional<float> timeToDefuseEnd));
    MOCK_METHOD(MockDefusingCountdownTextPanel&, setCanBeDefused, (std::optional<bool> canBeDefused));
};
