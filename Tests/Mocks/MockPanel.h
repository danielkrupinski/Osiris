#pragma once

#include <gmock/gmock.h>

struct MockPanel {
    MOCK_METHOD(void, setParent, (const MockPanel& parent));
};
