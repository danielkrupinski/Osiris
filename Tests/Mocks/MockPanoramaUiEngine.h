#pragma once

#include <gmock/gmock.h>
#include <CS2/Panorama/PanelHandle.h>

struct MockPanel;

struct MockPanoramaUiEngine {
    MOCK_METHOD(void, deletePanelByHandle, (cs2::PanelHandle handle));
    MOCK_METHOD(MockPanel&, getPanelFromHandle, (cs2::PanelHandle handle));
    MOCK_METHOD(void, runScript, (MockPanel& contextPanel, const char* script));
};
