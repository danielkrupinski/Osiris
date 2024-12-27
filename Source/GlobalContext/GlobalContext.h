#pragma once

#include <cstddef>
#include <span>

#include <BuildConfig.h>
#include <CS2/Constants/DllNames.h>
#include <Features/Common/InWorldPanelsUnloadHandler.h>
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
                dependencies.make<PanoramaGUI>().init(dependencies.make<PanoramaUiPanel>((*mainMenu)->uiPanel));
            fullCtx.hooks.peepEventsHook.disable();
            fullCtx.hooks.viewRenderHook.install();
        }
        return PeepEventsHookResult{fullCtx.hooks.peepEventsHook.original};
    }

    [[nodiscard]] std::uint64_t playerPawnSceneObjectUpdater(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept
    {
        HookDependencies hookContext{fullContext()};
        const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater(playerPawn, unknown, unknownBool);
        hookContext.make<ModelGlow>().applyPlayerModelGlow(hookContext.make<PlayerPawn>(playerPawn));
        return originalReturnValue;
    }

    [[nodiscard]] std::uint64_t weaponSceneObjectUpdater(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept
    {
        HookDependencies hookContext{fullContext()};
        const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater(weapon, unknown, unknownBool);
        hookContext.make<ModelGlow>().applyWeaponModelGlow(hookContext.make<BaseWeapon>(weapon));
        return originalReturnValue;
    }

    [[nodiscard]] UnloadFlag onRenderStartHook(cs2::CViewRender* viewRender) noexcept
    {
        HookDependencies dependencies{fullContext()};
        fullContext().hooks.viewRenderHook.getOriginalOnRenderStart()(viewRender);
        SoundWatcher<decltype(dependencies)> soundWatcher{fullContext().soundWatcherState, dependencies};
        soundWatcher.update();
        fullContext().features(dependencies).soundFeatures().runOnViewMatrixUpdate();

        dependencies.make<RenderingHookEntityLoop>().run();
        dependencies.make<GlowSceneObjects>().removeUnreferencedObjects();
        dependencies.make<DefusingAlert>().run();
        dependencies.make<KillfeedPreserver>().run();
        dependencies.make<BombStatusPanelManager>().run();
        dependencies.make<InWorldPanels>().hideUnusedPanels();

        UnloadFlag unloadFlag;
        dependencies.make<PanoramaGUI>().run(fullContext().features(dependencies), unloadFlag);

        if (unloadFlag) {
            FeaturesUnloadHandler{dependencies, fullContext().featuresStates}.handleUnload();
            BombStatusPanelUnloadHandler{dependencies}.handleUnload();
            InWorldPanelsUnloadHandler{dependencies}.handleUnload();
            PanoramaGuiUnloadHandler{dependencies}.handleUnload();
            fullContext().hooks.viewRenderHook.uninstall();

            dependencies.make<EntitySystem>().iterateEntities([&dependencies](auto& entity) {
                auto&& baseEntity = dependencies.make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(&entity));
                dependencies.make<ModelGlow>().onUnload(baseEntity.classify(), baseEntity);
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
