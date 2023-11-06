#pragma once

#include <CS2/Classes/Panorama.h>
#include <Platform/Macros/IsPlatform.h>
#include "Implementation/PanoramaUiEngineImpl.h"

struct PanoramaUiEngine {
    static void runScript(cs2::CUIPanel* contextPanel, const char* scriptSource, const char* originFile, std::uint64_t line) noexcept
    {
        if (impl().runScript && impl().thisptr)
            impl().runScript(*impl().thisptr, contextPanel, scriptSource, originFile, line);
    }

    static cs2::CUIPanel* getPanelPointer(cs2::PanelHandle handle) noexcept
    {
        if (impl().getPanelPointer && impl().thisptr)
            return impl().getPanelPointer(*impl().thisptr, &handle);
        return nullptr;
    }

    static cs2::PanelHandle getPanelHandle(cs2::CUIPanel* panel) noexcept
    {
        cs2::PanelHandle handle{};
        if (impl().getPanelHandle && impl().thisptr) {
#if IS_WIN64()
            impl().getPanelHandle(*impl().thisptr, &handle, panel);
#elif IS_LINUX()
            handle = impl().getPanelHandle(*impl().thisptr, panel);
#endif
        }
        return handle;
    }

    static void onDeletePanel(cs2::PanelHandle panelHandle) noexcept
    {
        if (impl().onDeletePanel && impl().thisptr)
            impl().onDeletePanel(*impl().thisptr, &panelHandle);
    }

    static cs2::CPanoramaSymbol makeSymbol(int type, const char* text) noexcept
    {
        if (impl().makeSymbol && impl().thisptr)
            return impl().makeSymbol(*impl().thisptr, type, text);
        return -1;
    }

private:
    [[nodiscard]] static const PanoramaUiEngineImpl& impl() noexcept
    {
        return PanoramaUiEngineImpl::instance();
    }
};
