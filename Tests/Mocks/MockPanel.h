#pragma once

#include <gmock/gmock.h>

struct MockPanel {
    MOCK_METHOD(void, setParent, (const MockPanel& parent));
    MOCK_METHOD(void, show, ());
    MOCK_METHOD(void, hide, ());
};
