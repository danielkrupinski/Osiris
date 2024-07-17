#pragma once

#include <gmock/gmock.h>

struct MockBombTimerTextPanel {
    MOCK_METHOD(void, setTimeToExplosion, (float timeToExplosion));
};
