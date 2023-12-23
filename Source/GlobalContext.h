#pragma once

#include <optional>

#include "CS2/Constants/DllNames.h"
#include "Hooks/Hooks.h"
#include "Hooks/PeepEventsHook.h"
#include "Hooks/ViewRenderHook.h"

#include <MemorySearch/PatternFinder.h>
#include <Features/Features.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/FeatureHelpers.h>
#include <UI/Panorama/PanoramaGUI.h>
#include "Platform/DynamicLibrary.h"
#include "Platform/VmtFinder.h"

#include "GameClasses/Implementation/GameClassImplementations.h"
#include "Helpers/UnloadFlag.h"
#include "Utils/LazyInitialized.h"

#include "BuildConfig.h"

struct GlobalContext {
    PeepEventsHook peepEventsHook{ DynamicLibrary{cs2::SDL_DLL}.getFunctionAddress("SDL_PeepEvents").add(WIN32_LINUX(3, 2)).abs().as<cs2::SDL_PeepEvents*>() };
    UnloadFlag unloadFlag;
    FreeMemoryRegionList freeRegionList{ storage };
    LazyInitialized<GameClassImplementations> gameClasses;
    LazyInitialized<Hooks> hooks;
    LazyInitialized<SoundWatcher> soundWatcher;
    LazyInitialized<FeatureHelpers> featureHelpers;
    LazyInitialized<Features> features;
    LazyInitialized<PanoramaGUI> panoramaGUI;

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

        const DynamicLibrary panoramaDLL{cs2::PANORAMA_DLL};
        const DynamicLibrary clientDLL{cs2::CLIENT_DLL};

        const PatternFinder clientPatternFinder{clientDLL.getCodeSection().raw(), PatternNotFoundLogger{}};
        const PatternFinder panoramaPatternFinder{panoramaDLL.getCodeSection().raw(), PatternNotFoundLogger{}};
        const PatternFinder soundSystemPatternFinder{DynamicLibrary{cs2::SOUNDSYSTEM_DLL}.getCodeSection().raw(), PatternNotFoundLogger{}};
        const PatternFinder fileSystemPatternFinder{DynamicLibrary{cs2::FILESYSTEM_DLL}.getCodeSection().raw(), PatternNotFoundLogger{}};

        const FileSystemPatterns fileSystemPatterns{soundSystemPatternFinder, fileSystemPatternFinder};
        gameClasses.init(ClientModePatterns{clientPatternFinder},
                         ClientPatterns{clientPatternFinder},
                         fileSystemPatterns,
                         GameRulesPatterns{clientPatternFinder},
                         MemAllocPatterns{clientPatternFinder},
                         PanelPatterns{clientPatternFinder},
                         PanelStylePatterns{panoramaPatternFinder},
                         PanoramaImagePanelPatterns{clientPatternFinder},
                         PanoramaLabelPatterns{clientPatternFinder},
                         PanoramaUiEnginePatterns{panoramaPatternFinder},
                         PanoramaUiPanelPatterns{clientPatternFinder, panoramaPatternFinder},
                         PlantedC4Patterns{clientPatternFinder},
                         Tier0Dll{});

        const VmtLengthCalculator clientVmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()};
        hooks.init(ClientPatterns{clientPatternFinder}, clientVmtLengthCalculator);
        soundWatcher.init(fileSystemPatterns, SoundSystemPatterns{soundSystemPatternFinder});
        featureHelpers.init(ClientPatterns{clientPatternFinder}, PanelStylePatterns{panoramaPatternFinder}, VmtFinder{panoramaDLL.getVmtFinderParams()});
        features.init(ClientPatterns{clientPatternFinder}, featureHelpers->sniperScopeBlurRemover, hooks->loopModeGameHook, hooks->viewRenderHook, *soundWatcher);
        panoramaGUI.init(ClientPatterns{clientPatternFinder});

        initializedFromGameThread = true;
    }

    void enableIfValid()
    {
        if (peepEventsHook.isValid())
            peepEventsHook.enable();
    }

    void onRenderStart(cs2::CViewRender* thisptr) noexcept
    {
        hooks->viewRenderHook.getOriginalOnRenderStart()(thisptr);
        if (featureHelpers->globalVarsProvider && featureHelpers->globalVarsProvider.getGlobalVars())
            soundWatcher->update(featureHelpers->globalVarsProvider.getGlobalVars()->curtime);
        features->soundFeatures.runOnViewMatrixUpdate(featureHelpers->getSoundVisualizationHelpers());
    }

private:
    bool initializedFromGameThread = false;
    static constinit ManuallyDestructible<GlobalContext> globalContext;
    alignas(FreeMemoryRegionList::minimumAlignment()) static constinit std::byte storage[build::MEMORY_CAPACITY];
};
