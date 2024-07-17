#pragma once

#include <gmock/gmock.h>

struct MockDeathNotice {
    MOCK_METHOD(bool, isLocalPlayerKiller, ());
    MOCK_METHOD(bool, wasSpawnedThisRound, ());
    MOCK_METHOD(void, markAsJustSpawned, ());
};
