#pragma once

#include <Platform/Macros/FunctionAttributes.h>
#include <Platform/Macros/IsPlatform.h>
#include <MemoryPatterns/PatternTypes/UiEnginePatternTypes.h>

template <typename HookContext, typename Context>
struct PanoramaUiPanel;

template <typename HookContext>
class PanoramaUiEngine {
public:
    explicit PanoramaUiEngine(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void runScript(cs2::CUIPanel* contextPanel, const char* scriptSource) noexcept
    {
        // Game update on 3 October 2024 added panorama script caching (panorama_script_cache_enabled convar)
        // By setting 'line' to non-zero value we disable caching so we don't have to specify different file path for different scripts
        constexpr auto originFile{'\0'};
        constexpr auto line{1};
        if (hookContext.panoramaPatternSearchResults().template get<RunScriptFunctionPointer>() && thisptr())
            hookContext.panoramaPatternSearchResults().template get<RunScriptFunctionPointer>()(*thisptr(), contextPanel, scriptSource, &originFile, line);
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

    [[NOINLINE]] void deletePanelByHandle(cs2::PanelHandle handle) noexcept
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
        cs2::CPanel2D* clientPanel = getPanelFromHandle(panelHandle).clientPanel();
        if (clientPanel && hookContext.panoramaPatternSearchResults().template get<OnDeletePanelFunctionPointer>() && thisptr())
            hookContext.panoramaPatternSearchResults().template get<OnDeletePanelFunctionPointer>()(*thisptr(), clientPanel);
    }

    HookContext& hookContext;
};
