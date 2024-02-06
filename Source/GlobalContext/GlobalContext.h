#pragma once

#include <cstddef>
#include <span>
#include <variant>

#include <BuildConfig.h>
#include <CS2/Constants/DllNames.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemoryAllocation/FreeMemoryRegionList.h>
#include <MemorySearch/PatternFinder.h>
#include <Platform/DynamicLibrary.h>
#include <SDL/SdlDll.h>

#include "FullGlobalContext.h"
#include "PartialGlobalContext.h"

class GlobalContext {
public:
    explicit GlobalContext(std::span<std::byte> memoryStorage) noexcept
        : _freeRegionList{memoryStorage}
    {
    }

    static void initializeInstance() noexcept
    {
        if (!globalContext.isInitialized()) {
            alignas(FreeMemoryRegionList::minimumAlignment()) static constinit std::byte storage[build::MEMORY_CAPACITY];
            globalContext.initialize(storage);
            const SdlDll sdlDll;
            const PatternFinder<PatternNotFoundLogger> sdlPatternFinder{sdlDll.getCodeSection().raw()};
            globalContext->variantContext.emplace<PartialGlobalContext>(sdlDll, SdlPatterns{sdlPatternFinder}).enableIfValid();
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
        assert(std::holds_alternative<FullGlobalContext>(variantContext));
        return *std::get_if<FullGlobalContext>(&variantContext);
    }

    [[nodiscard]] PeepEventsHookResult peepEventsHook() noexcept
    {
        initializeFullContextFromGameThread();
        return fullContext().onPeepEventsHook();
    }

private:
    void initializeFullContextFromGameThread() noexcept
    {
        if (!std::holds_alternative<PartialGlobalContext>(variantContext))
            return;

        const DynamicLibrary panoramaDLL{cs2::PANORAMA_DLL};
        const DynamicLibrary clientDLL{cs2::CLIENT_DLL};

        const PatternFinder<PatternNotFoundLogger> clientPatternFinder{clientDLL.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> panoramaPatternFinder{panoramaDLL.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> soundSystemPatternFinder{DynamicLibrary{cs2::SOUNDSYSTEM_DLL}.getCodeSection().raw()};
        const PatternFinder<PatternNotFoundLogger> fileSystemPatternFinder{DynamicLibrary{cs2::FILESYSTEM_DLL}.getCodeSection().raw()};

        const FileSystemPatterns fileSystemPatterns{soundSystemPatternFinder, fileSystemPatternFinder};

        variantContext.emplace<FullGlobalContext>(
            std::get_if<PartialGlobalContext>(&variantContext)->peepEventsHook,
            clientDLL,
            panoramaDLL,
            clientPatternFinder, 
            panoramaPatternFinder, 
            soundSystemPatternFinder, 
            fileSystemPatterns
        );

        fullContext().panoramaGUI.init(fullContext().getFeatureHelpers().mainMenuProvider);
    }

    FreeMemoryRegionList _freeRegionList;
    std::variant<std::monostate, PartialGlobalContext, FullGlobalContext> variantContext;

    static constinit ManuallyDestructible<GlobalContext> globalContext;
};
