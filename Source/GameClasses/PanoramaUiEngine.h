#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <Platform/Macros/IsPlatform.h>
#include <MemoryPatterns/PatternTypes/UiEnginePatternTypes.h>

template <typename Context>
struct PanoramaUiPanel;

template <typename HookContext>
class PanoramaUiEngine {
public:
    explicit PanoramaUiEngine(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void runScript(cs2::CUIPanel* contextPanel, const char* scriptSource, const char* originFile, std::uint64_t line) noexcept
    {
        if (hookContext.panoramaPatternSearchResults().template get<RunScriptFunctionPointer>() && thisptr())
            hookContext.panoramaPatternSearchResults().template get<RunScriptFunctionPointer>()(*thisptr(), contextPanel, scriptSource, originFile, line);
    }

    [[nodiscard]] decltype(auto) getPanelFromHandle(cs2::PanelHandle handle) noexcept
    {
        if (hookContext.panoramaPatternSearchResults().template get<GetPanelPointerFunctionPointer>() && thisptr())
            return hookContext.template make<PanoramaUiPanel>(hookContext.panoramaPatternSearchResults().template get<GetPanelPointerFunctionPointer>()(*thisptr(), &handle));
        return hookContext.template make<PanoramaUiPanel>(nullptr);
    }

    [[nodiscard]] cs2::PanelHandle getPanelHandle(cs2::CUIPanel* panel) noexcept
    {
        cs2::PanelHandle handle{};
        if (hookContext.panoramaPatternSearchResults().template get<GetPanelHandleFunctionPointer>() && thisptr()) {
#if IS_WIN64()
            hookContext.panoramaPatternSearchResults().template get<GetPanelHandleFunctionPointer>()(*thisptr(), &handle, panel);
#elif IS_LINUX()
            handle = hookContext.panoramaPatternSearchResults().template get<GetPanelHandleFunctionPointer>()(*thisptr(), panel);
#endif
        }
        return handle;
    }

    void deletePanelByHandle(cs2::PanelHandle handle) noexcept
    {
        if (handle.isValid())
            onDeletePanel(handle);
    }

    [[nodiscard]] cs2::CPanoramaSymbol makeSymbol(int type, const char* text) noexcept
    {
        if (hookContext.panoramaPatternSearchResults().template get<MakeSymbolFunctionPointer>() && thisptr())
#if IS_WIN64()
            return hookContext.panoramaPatternSearchResults().template get<MakeSymbolFunctionPointer>()(type, text);
#elif IS_LINUX()
            return hookContext.panoramaPatternSearchResults().template get<MakeSymbolFunctionPointer>()(*thisptr(), type, text);
#endif
        return -1;
    }

private:
    [[nodiscard]] auto thisptr() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<UiEnginePointer>();
    }

    void onDeletePanel(cs2::PanelHandle panelHandle) noexcept
    {
        if (hookContext.panoramaPatternSearchResults().template get<OnDeletePanelFunctionPointer>() && thisptr())
            hookContext.panoramaPatternSearchResults().template get<OnDeletePanelFunctionPointer>()(*thisptr(), &panelHandle);
    }

    HookContext& hookContext;
};
