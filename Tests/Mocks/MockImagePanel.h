#pragma once

#include <gmock/gmock.h>

struct MockPanel;

struct MockImagePanel {
    MOCK_METHOD(void, setImageSvg, (const char* imageUrl));
    MOCK_METHOD(void, setImageSvg, (const char* imageUrl, int textureHeight));
    MOCK_METHOD(MockPanel&, uiPanel, ());
};
