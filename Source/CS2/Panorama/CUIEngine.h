#pragma once

#include <cstdint>

#include <CS2/Classes/CUtlAbstractDelegate.h>
#include <Platform/Macros/IsPlatform.h>
#include "CPanel2D.h"
#include "CPanoramaSymbol.h"
#include "CUIPanel.h"
#include "PanelHandle.h"

namespace cs2
{

struct CUIEngine {
    using getPanelPointer = CUIPanel* (*)(CUIEngine* thisptr, const PanelHandle* handle);
    using runScript = void (*)(CUIEngine* thisptr, CUIPanel* contextPanel, const char* scriptSource, const char* originFile, std::uint64_t line);
    using makeSymbol = CPanoramaSymbol(*)(CUIEngine* thisptr, int type, const char* text);
    using onDeletePanel = void (*)(CUIEngine* thisptr, CPanel2D* panel);
    using RegisterEventHandler = void (*)(CUIEngine* thisptr, CPanoramaSymbol eventName, CUIPanel* panel, CUtlAbstractDelegate* handler);
};

}
