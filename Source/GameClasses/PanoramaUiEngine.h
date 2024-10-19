#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <Platform/Macros/IsPlatform.h>
#include <GameDependencies/PanoramaUiEngineDeps.h>
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
        if (deps().runScript && thisptr())
            deps().runScript(*thisptr(), contextPanel, scriptSource, originFile, line);
    }

    [[nodiscard]] decltype(auto) getPanelFromHandle(cs2::PanelHandle handle) noexcept
    {
        if (deps().getPanelPointer && thisptr())
            return hookContext.template make<PanoramaUiPanel>(deps().getPanelPointer(*thisptr(), &handle));
        return hookContext.template make<PanoramaUiPanel>(nullptr);
    }

    [[nodiscard]] cs2::PanelHandle getPanelHandle(cs2::CUIPanel* panel) noexcept
    {
        cs2::PanelHandle handle{};
        if (deps().getPanelHandle && thisptr()) {
#if IS_WIN64()
            deps().getPanelHandle(*thisptr(), &handle, panel);
#elif IS_LINUX()
            handle = deps().getPanelHandle(*thisptr(), panel);
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
        if (deps().makeSymbol && thisptr())
#if IS_WIN64()
            return deps().makeSymbol(type, text);
#elif IS_LINUX()
            return deps().makeSymbol(*thisptr(), type, text);
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
        if (deps().onDeletePanel && thisptr())
            deps().onDeletePanel(*thisptr(), &panelHandle);
    }

    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().uiEngineDeps;
    }

    HookContext& hookContext;
};
