#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <BuildConfig.h>
#include <CS2/Classes/CViewRender.h>
#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Constants/DllNames.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/BaseWeapon.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/PreviewPlayer.h>
#include <Features/Hud/DefusingAlert/DefusingAlert.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserver.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreview.h>
#include <Features/Common/InWorldPanelsUnloadHandler.h>
#include <MemorySearch/PatternNotFoundLogger.h>
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
            fullCtx.entityClassifier.init(dependencies);
            if (const auto mainMenu{fullCtx.clientPatternSearchResults.get<MainMenuPanelPointer>()}; mainMenu && *mainMenu)
                dependencies.make<PanoramaGUI>().init(dependencies.make<PanoramaUiPanel>((*mainMenu)->uiPanel));
            fullCtx.hooks.peepEventsHook.disable();
            fullCtx.hooks.viewRenderHook.install();
        }
        return PeepEventsHookResult{fullCtx.hooks.peepEventsHook.original};
    }

    [[nodiscard]] std::uint64_t playerPawnSceneObjectUpdater(cs2::C_CSPlayerPawn* entity, void* unknown, bool unknownBool) noexcept
    {
        HookDependencies hookContext{fullContext()};
        const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater(entity, unknown, unknownBool);

        auto&& playerPawn = hookContext.make<PlayerPawn>(entity);
        if (auto&& previewPlayer = playerPawn.template cast<PreviewPlayer>(); !previewPlayer)
            hookContext.make<ModelGlow>().applyPlayerModelGlow(playerPawn);
        else
            hookContext.make<PlayerModelGlowPreview>().applyPreviewPlayerModelGlow(previewPlayer);
    
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
        dependencies.make<InWorldPanels>().updateState();
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
            dependencies.make<PlayerModelGlowPreview>().onUnload();

            dependencies.make<EntitySystem>().forEachEntityIdentity([&dependencies](const auto& entityIdentity) {
                auto&& baseEntity = dependencies.make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));
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
            MemoryPatterns{partialContext.patternFinders},
            Tier0Dll{}
        );

        return true;
    }

    FreeMemoryRegionList _freeRegionList;
    DeferredCompleteObject<PartialGlobalContext, FullGlobalContext> deferredCompleteContext;

    static constinit ManuallyDestructible<GlobalContext> globalContext;
};
