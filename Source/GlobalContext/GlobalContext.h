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

class GlobalContext {
public:
    explicit GlobalContext(std::span<std::byte> memoryStorage, SdlDll sdlDll, SdlPatterns sdlPatterns) noexcept
        : _freeRegionList{memoryStorage}
        , deferredCompleteContext{sdlDll, sdlPatterns}
    {
    }

    static void initializeInstance() noexcept
    {
        if (!globalContext.isInitialized()) {
            alignas(FreeMemoryRegionList::minimumAlignment()) static constinit std::byte storage[build::MEMORY_CAPACITY];
            const SdlDll sdlDll;
            const PatternFinder<PatternNotFoundLogger> sdlPatternFinder{sdlDll.getCodeSection().raw()};
            globalContext.initialize(storage, sdlDll, SdlPatterns{sdlPatternFinder});
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
        initializeCompleteContextFromGameThread();
        return fullContext().onPeepEventsHook();
    }

private:
    void initializeCompleteContextFromGameThread() noexcept
    {
        if (deferredCompleteContext.isComplete())
            return;

        const DynamicLibrary panoramaDLL{cs2::PANORAMA_DLL};
        const DynamicLibrary clientDLL{cs2::CLIENT_DLL};

        const PatternFinder<PatternNotFoundLogger> clientPatternFinder{clientDLL.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> panoramaPatternFinder{panoramaDLL.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> soundSystemPatternFinder{DynamicLibrary{cs2::SOUNDSYSTEM_DLL}.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> fileSystemPatternFinder{DynamicLibrary{cs2::FILESYSTEM_DLL}.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> tier0PatternFinder{DynamicLibrary{cs2::TIER0_DLL}.getCodeSection().raw()};

        const FileSystemPatterns fileSystemPatterns{soundSystemPatternFinder, fileSystemPatternFinder};

        deferredCompleteContext.makeComplete(
            deferredCompleteContext.partial().peepEventsHook,
            clientDLL,
            panoramaDLL,
            clientPatternFinder,
            panoramaPatternFinder,
            soundSystemPatternFinder,
            tier0PatternFinder,
            fileSystemPatterns
        );

        fullContext().panoramaGUI.init(fullContext().getFeatureHelpers().mainMenuProvider);
    }

    FreeMemoryRegionList _freeRegionList;
    DeferredCompleteObject<PartialGlobalContext, FullGlobalContext> deferredCompleteContext;

    static constinit ManuallyDestructible<GlobalContext> globalContext;
};
