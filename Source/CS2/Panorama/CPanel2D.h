#pragma once

#include "PanelHandle.h"

namespace cs2
{

struct CUIPanel;

struct CPanel2D {
    const void* vmt;
    CUIPanel* uiPanel;
    PanelHandle tooltipHandle;
    void* v8Pointer;

    using Create = CPanel2D*(const char* id, CPanel2D* parent);
};

};
