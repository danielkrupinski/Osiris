#pragma once

#include <Platform/Macros/IsPlatform.h>

namespace cs2
{

struct CUIEngine {
#if IS_WIN64()
    using getPanelHandle = void (*)(CUIEngine* thisptr, PanelHandle* handle, CUIPanel* panel);
#elif IS_LINUX()
    using getPanelHandle = PanelHandle (*)(CUIEngine* thisptr, CUIPanel* panel);
#endif
    using getPanelPointer = CUIPanel* (*)(CUIEngine* thisptr, const PanelHandle* handle);
    using runScript = void (*)(CUIEngine* thisptr, CUIPanel* contextPanel, const char* scriptSource, const char* originFile, std::uint64_t line);
    using makeSymbol = CPanoramaSymbol (*)(CUIEngine* thisptr, int type, const char* text);
    using onDeletePanel = void (*)(CUIEngine* thisptr, PanelHandle* panelHandle);
};

}
