#pragma once

#include <gmock/gmock.h>

struct MockDefusingAlertPanel {
    MOCK_METHOD(void, showAndUpdate, ());
    MOCK_METHOD(void, hide, ());
};
