#pragma once

struct MockGameBombStatusPanel {
    MOCK_METHOD(void, hide, ());
    MOCK_METHOD(void, restore, ());
};
