#pragma once

namespace cs2
{

struct CUIPanel;

struct CPanel2D {
    const void* vmt;
    CUIPanel* uiPanel;

    using Create = CPanel2D*(const char* id, CPanel2D* parent);
};

};
