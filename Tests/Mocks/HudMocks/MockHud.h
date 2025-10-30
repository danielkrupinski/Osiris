#pragma once

#include <gmock/gmock.h>

struct MockDeathNotices;
struct MockPanel;

struct MockHud {
    MOCK_METHOD(MockDeathNotices&, deathNotices, ());
    MOCK_METHOD(MockPanel&, timerTextPanel, ());
    MOCK_METHOD(MockPanel&, getHudReticle, ());
    MOCK_METHOD(MockPanel&, scoreAndTimeAndBomb, ());
};
