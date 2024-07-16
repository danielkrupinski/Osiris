#pragma once

#include <functional>

#include "MockDeathNotice.h"

struct MockDeathNotices {
    MOCK_METHOD(void, forEach, (const std::function<void(MockDeathNotice&)>&));
};
