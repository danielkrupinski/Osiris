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
            if (const auto mainMenu{fullCtx.clientPatternSearchResults.get<MainMenuPanelPointer>()}; mainMenu && *mainMenu)
                dependencies.make<PanoramaGUI>().init(PanoramaUiPanel{PanoramaUiPanelContext{dependencies, (*mainMenu)->uiPanel}});
            fullCtx.hooks.peepEventsHook.disable();
            fullCtx.hooks.viewRenderHook.install();
        }
        return PeepEventsHookResult{fullCtx.hooks.peepEventsHook.original};
    }

    [[nodiscard]] std::uint64_t playerPawnSceneObjectUpdater(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept
    {
        HookDependencies hookContext{fullContext()};
        const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater(playerPawn, unknown, unknownBool);
        hookContext.make<ModelGlow>().applyModelGlow(hookContext.make<PlayerPawn>(playerPawn));
        return originalReturnValue;
    }

    [[nodiscard]] UnloadFlag onRenderStartHook(cs2::CViewRender* viewRender) noexcept
    {
        HookDependencies dependencies{fullContext()};
        fullContext().hooks.viewRenderHook.getOriginalOnRenderStart()(viewRender);
        SoundWatcher<decltype(dependencies)> soundWatcher{fullContext().soundWatcherState, dependencies};
        soundWatcher.update();
        fullContext().features(dependencies).soundFeatures().runOnViewMatrixUpdate();

        auto&& playerInfoInWorld = dependencies.make<PlayerInfoInWorld>();
        RenderingHookEntityLoop{dependencies, playerInfoInWorld}.run();
        playerInfoInWorld.hideUnusedPanels();
        dependencies.make<GlowSceneObjects>().removeUnreferencedObjects();

        fullContext().features(dependencies).hudFeatures().defusingAlert().run();
        fullContext().features(dependencies).hudFeatures().killfeedPreserver().run();
        BombStatusPanelManager{BombStatusPanelManagerContext{dependencies}}.run();

        UnloadFlag unloadFlag;
        dependencies.make<PanoramaGUI>().run(fullContext().features(dependencies), unloadFlag);

        if (unloadFlag) {
            FeaturesUnloadHandler{dependencies, fullContext().featuresStates}.handleUnload();
            BombStatusPanelUnloadHandler{dependencies}.handleUnload();
            InWorldPanelContainerUnloadHandler{dependencies}.handleUnload();
            PanoramaGuiUnloadHandler{dependencies}.handleUnload();
            fullContext().hooks.viewRenderHook.uninstall();

            dependencies.make<EntitySystem>().iterateEntities([&dependencies](auto& entity) {
                auto&& baseEntity = dependencies.make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(&entity));
                const auto entityTypeInfo = dependencies.entityClassifier().classifyEntity(dependencies.gameDependencies().entitiesVMTs, entity.vmt);

                if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
                    dependencies.make<ModelGlow>().onUnload(baseEntity.as<PlayerPawn>());
            });
        }
        return unloadFlag;
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
