#pragma once

#include <gmock/gmock.h>

struct MockImagePanel {
    MOCK_METHOD(void, setImageSvg, (const char* imageUrl));
};
