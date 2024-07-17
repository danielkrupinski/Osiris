#pragma once

#include <functional>

#include <gmock/gmock.h>

struct KillfeedPreserverState;
struct MockDeathNotice;
struct MockDeathNotices;

struct MockKillfeedPreserverContext {
    MOCK_METHOD(KillfeedPreserverState&, state, ());
    MOCK_METHOD(MockDeathNotices&, deathNotices, ());
    MOCK_METHOD(const std::function<void(MockDeathNotice&)>&, preserveDeathNotice, ());
};
