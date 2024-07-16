#pragma once

#include <gmock/gmock.h>

struct MockDeathNotices;

struct MockHud {
    MOCK_METHOD(MockDeathNotices&, deathNotices, ());
};
