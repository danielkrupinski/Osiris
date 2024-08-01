#pragma once

#include <optional>

#include <gmock/gmock.h>

struct MockPanel {
    MOCK_METHOD(void, setParent, (const MockPanel& parent));
    MOCK_METHOD(void, show, ());
    MOCK_METHOD(void, hide, ());
    MOCK_METHOD(std::optional<bool>, isVisible, ());
};
