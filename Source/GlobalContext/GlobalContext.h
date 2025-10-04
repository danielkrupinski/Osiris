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
#include <Features/Sound/SoundFeatures.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreview.h>
#include <MemorySearch/PatternNotFoundLogger.h>
#include <MemoryAllocation/FreeMemoryRegionList.h>
#include <MemorySearch/PatternFinder.h>
#include <Platform/DynamicLibrary.h>
#include <SDL/SdlDll.h>

#include "DeferredCompleteObject.h"
#include "FullGlobalContext.h"
#include "PartialGlobalContext.h"
#include "HookContext/HookContext.h"

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
        HookContext hookContext{fullCtx};

        if (justInitialized) {
            fullCtx.entityClassifier.init(hookContext);
            if (const auto mainMenu{fullCtx.patternSearchResults.get<MainMenuPanelPointer>()}; mainMenu && *mainMenu)
                hookContext.make<PanoramaGUI>().init(hookContext.make<PanoramaUiPanel>((*mainMenu)->uiPanel));
            hookContext.config().init();
            hookContext.config().scheduleLoad();
            fullCtx.hooks.peepEventsHook.disable();
            fullCtx.hooks.viewRenderHook.install();
        }
        return PeepEventsHookResult{fullCtx.hooks.peepEventsHook.original};
    }

    [[nodiscard]] std::uint64_t playerPawnSceneObjectUpdater(cs2::C_CSPlayerPawn* entity, void* unknown, bool unknownBool) noexcept
    {
        HookContext hookContext{fullContext()};
        const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater(entity, unknown, unknownBool);

        auto&& playerPawn = hookContext.make<PlayerPawn>(entity);
        if (auto&& previewPlayer = playerPawn.template cast<PreviewPlayer>(); !previewPlayer)
            hookContext.make<ModelGlow>().updateInSceneObjectUpdater()(PlayerModelGlow{hookContext}, playerPawn, EntityTypeInfo{});
        else
            hookContext.make<PlayerModelGlowPreview>().applyPreviewPlayerModelGlow(previewPlayer);
    
        return originalReturnValue;
    }

    [[nodiscard]] std::uint64_t weaponSceneObjectUpdater(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept
    {
        HookContext hookContext{fullContext()};
        const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater(weapon, unknown, unknownBool);
        if (auto&& c4 = hookContext.make<BaseWeapon>(weapon).template cast<C4>())
            hookContext.make<ModelGlow>().updateInSceneObjectUpdater()(DroppedBombModelGlow{hookContext}, c4.baseWeapon(), EntityTypeInfo{});
        else
            hookContext.make<ModelGlow>().updateInSceneObjectUpdater()(WeaponModelGlow{hookContext}, hookContext.make<BaseWeapon>(weapon), hookContext.make<BaseWeapon>(weapon).baseEntity().classify());
        return originalReturnValue;
    }

    [[nodiscard]] UnloadFlag onRenderStartHook(cs2::CViewRender* viewRender) noexcept
    {
        HookContext hookContext{fullContext()};
        fullContext().hooks.viewRenderHook.getOriginalOnRenderStart()(viewRender);
        hookContext.make<InWorldPanels>().updateState();
        SoundWatcher<decltype(hookContext)> soundWatcher{fullContext().soundWatcherState, hookContext};
        soundWatcher.update();
        SoundFeatures{hookContext.soundWatcherState(), fullContext().hooks.viewRenderHook, hookContext}.runOnViewMatrixUpdate();

        hookContext.make<RenderingHookEntityLoop>().run();
        hookContext.make<GlowSceneObjects>().removeUnreferencedObjects();
        hookContext.make<DefusingAlert>().run();
        hookContext.make<KillfeedPreserver>().run();
        hookContext.make<BombStatusPanelManager>().run();
        hookContext.make<InWorldPanels>().hideUnusedPanels();

        UnloadFlag unloadFlag;
        hookContext.make<PanoramaGUI>().run(unloadFlag);
        hookContext.config().update();
        hookContext.config().performFileOperation();

        if (unloadFlag) {
            hookContext.make<BombTimer>().onUnload();
            hookContext.make<DefusingAlert>().onUnload();
            hookContext.make<PostRoundTimer>().onUnload();
            hookContext.make<OutlineGlow>().onUnload();
            hookContext.make<BombStatusPanel>().onUnload();
            hookContext.make<InWorldPanels>().onUnload();
            hookContext.make<PanoramaGUI>().onUnload();
            fullContext().hooks.viewRenderHook.uninstall();
            hookContext.make<PlayerModelGlowPreview>().onUnload();
            hookContext.make<WeaponModelGlowPreview>().onUnload();

            hookContext.make<EntitySystem>().forEachNetworkableEntityIdentity([&hookContext](const auto& entityIdentity) {
                auto&& baseEntity = hookContext.make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));
                const auto entityTypeInfo = baseEntity.classify();
                if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
                    hookContext.make<ModelGlow>().onUnload()(PlayerModelGlow{hookContext}, baseEntity.template as<PlayerPawn>());
                else if (entityTypeInfo.is<cs2::C_C4>())
                    hookContext.make<ModelGlow>().onUnload()(DroppedBombModelGlow{hookContext}, baseEntity.template as<BaseWeapon>());
                else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
                    hookContext.make<ModelGlow>().onUnload()(DefuseKitModelGlow{hookContext}, baseEntity);
                else if (entityTypeInfo.is<cs2::CPlantedC4>())
                    hookContext.make<ModelGlow>().onUnload()(TickingBombModelGlow{hookContext}, baseEntity.template as<PlantedC4>());
                else if (entityTypeInfo.isGrenadeProjectile())
                    hookContext.make<ModelGlow>().onUnload()(GrenadeProjectileModelGlow{hookContext}, baseEntity);
                else if (entityTypeInfo.isWeapon())
                    hookContext.make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, baseEntity.template as<BaseWeapon>());
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
