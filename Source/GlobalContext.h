#pragma once

#include <optional>

#include "CS2/Constants/DllNames.h"
#include "Hooks/Hooks.h"
#include "Hooks/PeepEventsHook.h"
#include "Hooks/ViewRenderHook.h"

#include <MemorySearch/PatternFinder.h>
#include <Features/Features.h>
#include <UI/Panorama/PanoramaGUI.h>
#include "Platform/DynamicLibrary.h"

#include "GameClasses/Implementation/GameClassImplementations.h"
#include "Helpers/UnloadFlag.h"

#include "BuildConfig.h"

struct GlobalContext {
    PeepEventsHook peepEventsHook{ DynamicLibrary{cs2::SDL_DLL}.getFunctionAddress("SDL_PeepEvents").add(WIN32_LINUX(3, 2)).abs().as<cs2::SDL_PeepEvents*>() };
    UnloadFlag unloadFlag;
    FreeMemoryRegionList freeRegionList{ storage };
    PatternFinder<PatternNotFoundLogger> clientPatternFinder{ DynamicLibrary{cs2::CLIENT_DLL}.getCodeSection().raw(), PatternNotFoundLogger{} };
    PatternFinder<PatternNotFoundLogger> panoramaPatternFinder{ DynamicLibrary{cs2::PANORAMA_DLL}.getCodeSection().raw(), PatternNotFoundLogger{} };
    PatternFinder<PatternNotFoundLogger> soundSystemPatternFinder{ DynamicLibrary{cs2::SOUNDSYSTEM_DLL}.getCodeSection().raw(), PatternNotFoundLogger{} };
    PatternFinder<PatternNotFoundLogger> fileSystemPatternFinder{ DynamicLibrary{cs2::FILESYSTEM_DLL}.getCodeSection().raw(), PatternNotFoundLogger{} };
    std::optional<GameClassImplementations> gameClasses;
    std::optional<Hooks> hooks;
    std::optional<Features> features;
    std::optional<PanoramaGUI> panoramaGUI;

    static void initializeInstance() noexcept
    {
        if (!globalContext.isInitialized()) {
            globalContext.initialize();
            globalContext->enableIfValid();
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

    void initializeFromGameThread() noexcept
    {
        if (initializedFromGameThread)
            return;

        gameClasses.emplace();
        const VmtLengthCalculator clientVmtLengthCalculator{ DynamicLibrary{cs2::CLIENT_DLL}.getCodeSection(), DynamicLibrary{cs2::CLIENT_DLL}.getVmtSection() };
        hooks.emplace(clientVmtLengthCalculator);
        features.emplace(HudProvider{}, hooks->loopModeGameHook, hooks->viewRenderHook);
        panoramaGUI.emplace(*features, unloadFlag);

        initializedFromGameThread = true;
    }

    void enableIfValid()
    {
        if (peepEventsHook.isValid())
            peepEventsHook.enable();
    }

private:
    bool initializedFromGameThread = false;
    static constinit ManuallyDestructible<GlobalContext> globalContext;
    alignas(FreeMemoryRegionList::minimumAlignment()) static constinit std::byte storage[build::MEMORY_CAPACITY];
};
