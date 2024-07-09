#pragma once

#include <gmock/gmock.h>

struct MockLabelPanel {
    MOCK_METHOD(void, setText, (const char* text));
};
