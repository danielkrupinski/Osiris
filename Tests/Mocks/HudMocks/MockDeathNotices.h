#pragma once

#include <functional>

struct MockDeathNotice;

struct MockDeathNotices {
    MOCK_METHOD(void, forEach, (const std::function<void(MockDeathNotice&)>&));
};
