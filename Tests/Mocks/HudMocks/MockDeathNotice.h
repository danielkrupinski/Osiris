#pragma once

#include <gmock/gmock.h>
#include <Utils/Optional.h>

struct MockDeathNotice {
    MOCK_METHOD(bool, isLocalPlayerKiller, ());
    MOCK_METHOD(Optional<bool>, wasSpawnedThisRound, ());
    MOCK_METHOD(void, markAsJustSpawned, ());
};
