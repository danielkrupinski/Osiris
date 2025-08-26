#pragma once

#include <cstdint>

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
#if IS_WIN64()
    using makeSymbol = CPanoramaSymbol (*)(int type, const char* text);
#elif IS_LINUX()
    using makeSymbol = CPanoramaSymbol(*)(CUIEngine* thisptr, int type, const char* text);
#endif
    using onDeletePanel = void (*)(CUIEngine* thisptr, CPanel2D* panel);
};

}
