#pragma once

#include <gmock/gmock.h>
#include <CS2/Panorama/PanelHandle.h>

struct MockPanoramaUiEngine {
    MOCK_METHOD(void, deletePanelByHandle, (cs2::PanelHandle handle));
};
