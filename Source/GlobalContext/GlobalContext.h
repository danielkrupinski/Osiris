#pragma once

#include <cstddef>
#include <span>

#include <BuildConfig.h>
#include <CS2/Constants/DllNames.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemoryAllocation/FreeMemoryRegionList.h>
#include <MemorySearch/PatternFinder.h>
#include <Platform/DynamicLibrary.h>
#include <SDL/SdlDll.h>

#include "DeferredCompleteObject.h"
#include "FullGlobalContext.h"
#include "PartialGlobalContext.h"
#include "HookDependencies/HookDependencies.h"

class GlobalContext {
public:
    explicit GlobalContext(std::span<std::byte> memoryStorage, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, SdlDll sdlDll) noexcept
        : _freeRegionList{memoryStorage}
        , deferredCompleteContext{clientDLL, panoramaDLL, sdlDll}
    {
    }

    static void initializeInstance() noexcept
    {
        if (!globalContext.isInitialized()) {
            alignas(FreeMemoryRegionList::minimumAlignment()) static constinit std::byte storage[build::MEMORY_CAPACITY];
            globalContext.initialize(storage, DynamicLibrary{cs2::CLIENT_DLL}, DynamicLibrary{cs2::PANORAMA_DLL}, SdlDll{});
            globalContext->deferredCompleteContext.partial().enableIfValid();
        }
    }

    [[nodiscard]] static GlobalContext& instance() noexcept
    {
        return *globalContext;
    }

    static void destroyInstance() noexcept
    {
        globalContext.destroy();
    }

    [[nodiscard]] FreeMemoryRegionList& freeRegionList() noexcept
    {
        return _freeRegionList;
    }

    [[nodiscard]] FullGlobalContext& fullContext() noexcept
    {
        return deferredCompleteContext.complete();
    }

    [[nodiscard]] PeepEventsHookResult peepEventsHook() noexcept
    {
        const auto justInitialized = initializeCompleteContextFromGameThread();

        auto& fullCtx = fullContext();
        HookDependencies dependencies{fullCtx};

        if (justInitialized) {
            if (const auto mainMenu{fullCtx.gameDependencies.mainMenu}; mainMenu && *mainMenu)
                dependencies.make<PanoramaGUI>().init(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, (*mainMenu)->uiPanel}});
        }

        fullCtx.features(dependencies).hudFeatures().defusingAlert().run();
        fullCtx.features(dependencies).hudFeatures().killfeedPreserver().run();
        BombStatusPanelManager{BombStatusPanelManagerContext{dependencies}}.run();

        UnloadFlag unloadFlag;
        dependencies.make<PanoramaGUI>().run(fullCtx.features(dependencies), unloadFlag);
        fullCtx.hooks.update();

        if (unloadFlag) {
            FeaturesUnloadHandler{dependencies, fullCtx.featuresStates}.handleUnload();
            BombStatusPanelUnloadHandler{dependencies}.handleUnload();
            InWorldPanelContainerUnloadHandler{dependencies}.handleUnload();
            PanoramaGuiUnloadHandler{dependencies}.handleUnload();
            fullCtx.hooks.forceUninstall();
        }

        return PeepEventsHookResult{fullCtx.hooks.peepEventsHook.original, static_cast<bool>(unloadFlag)};
    }

private:
    bool initializeCompleteContextFromGameThread() noexcept
    {
        if (deferredCompleteContext.isComplete())
            return false;

        const auto partialContext = deferredCompleteContext.partial();

        deferredCompleteContext.makeComplete(
            partialContext.peepEventsHook,
            partialContext.clientDLL,
            partialContext.panoramaDLL,
            MemoryPatterns{partialContext.patternFinders}
        );

        return true;
    }

    FreeMemoryRegionList _freeRegionList;
    DeferredCompleteObject<PartialGlobalContext, FullGlobalContext> deferredCompleteContext;

    static constinit ManuallyDestructible<GlobalContext> globalContext;
};
