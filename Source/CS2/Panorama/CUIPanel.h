#pragma once

#include <cstdint>

#include <CS2/Classes/CUtlString.h>
#include <CS2/Classes/CUtlVector.h>

#include "CPanel2D.h"
#include "CPanoramaSymbol.h"

namespace cs2
{

enum EPanelFlag {
    k_EPanelFlag_IsVisible = 0x01,
    k_EPanelFlag_HasOwnLayoutFile = 0x40
};

struct CUIPanel {
    const void* vmt;
    CPanel2D* clientPanel;

    using setParent = void (*)(CUIPanel* thisptr, CUIPanel* parent);
    using setVisible = void (*)(CUIPanel* thisptr, bool visible);
    using getAttributeString = const char* (*)(CUIPanel* thisptr, CPanoramaSymbol attributeName, const char* defaultValue);
    using setAttributeString = void (*)(CUIPanel* thisptr, CPanoramaSymbol attributeName, const char* value);

    using childrenVector = CUtlVector<CUIPanel*>;
    using classesVector = CUtlVector<CPanoramaSymbol>;
    using m_pchID = CUtlString;
    using PanelFlags = std::uint8_t;
};

}
