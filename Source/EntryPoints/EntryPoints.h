#pragma once

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Utils/ReturnAddress.h"

[[NOINLINE]] void finishInit(auto& hookContext)
{
    hookContext.entityClassifier().init(hookContext);
    if (const auto mainMenu{hookContext.patternSearchResults().template get<MainMenuPanelPointer>()}; mainMenu && *mainMenu)
        hookContext.template make<PanoramaGUI>().init(hookContext.template make<PanoramaUiPanel>((*mainMenu)->uiPanel));
    hookContext.config().init();
    hookContext.config().scheduleLoad();
    hookContext.hooks().peepEventsHook.disable();
    hookContext.hooks().viewRenderHook.install();
}

int SDLHook_PeepEvents(void* events, int numevents, int action, unsigned minType, unsigned maxType) noexcept
{
    const auto initInProgress = !HookContext<GlobalContext>::isGlobalContextComplete();
    if (initInProgress)
        HookContext<GlobalContext>::initCompleteGlobalContextFromGameThread();

    HookContext<GlobalContext> hookContext;

    if (initInProgress)
        finishInit(hookContext);

    return hookContext.hooks().peepEventsHook.original(events, numevents, action, minType, maxType);
}

[[NOINLINE]] void unload(auto& hookContext) noexcept
{
    hookContext.template make<BombTimer>().onUnload();
    hookContext.template make<DefusingAlert>().onUnload();
    hookContext.template make<PostRoundTimer>().onUnload();
    hookContext.template make<OutlineGlow>().onUnload();
    hookContext.template make<BombStatusPanel>().onUnload();
    hookContext.template make<InWorldPanels>().onUnload();
    hookContext.template make<PanoramaGUI>().onUnload();
    hookContext.hooks().viewRenderHook.uninstall();
    hookContext.template make<ClientModeHooks>().restoreGetViewmodelFov();
    hookContext.template make<PlayerModelGlowPreview>().onUnload();
    hookContext.template make<WeaponModelGlowPreview>().onUnload();
    hookContext.template make<NoScopeInaccuracyVis>().onUnload();
    hookContext.template make<BombPlantAlert>().onUnload();

    hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([&hookContext](const auto& entityIdentity) {
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));
        const auto entityTypeInfo = baseEntity.classify();
        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
            hookContext.template make<ModelGlow>().onUnload()(PlayerModelGlow{hookContext}, baseEntity.template as<PlayerPawn>());
        else if (entityTypeInfo.template is<cs2::C_C4>())
            hookContext.template make<ModelGlow>().onUnload()(DroppedBombModelGlow{hookContext}, baseEntity.template as<BaseWeapon>());
        else if (entityTypeInfo.template is<cs2::CBaseAnimGraph>())
            hookContext.template make<ModelGlow>().onUnload()(DefuseKitModelGlow{hookContext}, baseEntity);
        else if (entityTypeInfo.template is<cs2::CPlantedC4>())
            hookContext.template make<ModelGlow>().onUnload()(TickingBombModelGlow{hookContext}, baseEntity.template as<PlantedC4>());
        else if (entityTypeInfo.isGrenadeProjectile())
            hookContext.template make<ModelGlow>().onUnload()(GrenadeProjectileModelGlow{hookContext}, baseEntity);
        else if (entityTypeInfo.isWeapon())
            hookContext.template make<ModelGlow>().onUnload()(WeaponModelGlow{hookContext}, baseEntity.template as<BaseWeapon>());
    });
}

void ViewRenderHook_onRenderStart(cs2::CViewRender* thisptr) noexcept
{
    HookContext<GlobalContext> hookContext;
    hookContext.hooks().viewRenderHook.getOriginalOnRenderStart()(thisptr);
    hookContext.make<InWorldPanels>().updateState();
    SoundWatcher<decltype(hookContext)> soundWatcher{hookContext.soundWatcherState(), hookContext};
    soundWatcher.update();
    SoundFeatures{hookContext.soundWatcherState(), hookContext.hooks().viewRenderHook, hookContext}.runOnViewMatrixUpdate();

    hookContext.make<NoScopeInaccuracyVis>().update();
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
        unload(hookContext);
        HookContext<GlobalContext>::destroyGlobalContext();
    }  
}

LINUX_ONLY([[gnu::aligned(8)]]) std::uint64_t PlayerPawn_sceneObjectUpdater(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept
{
    HookContext<GlobalContext> hookContext;
    const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater(playerPawn, unknown, unknownBool);

    auto&& playerPawn_ = hookContext.make<PlayerPawn>(playerPawn);
    if (auto&& previewPlayer = playerPawn_.template cast<PreviewPlayer>(); !previewPlayer)
        hookContext.make<ModelGlow>().updateInSceneObjectUpdater()(PlayerModelGlow{hookContext}, playerPawn_, EntityTypeInfo{});
    else
        hookContext.make<PlayerModelGlowPreview>().applyPreviewPlayerModelGlow(previewPlayer);

    return originalReturnValue;
}

LINUX_ONLY([[gnu::aligned(8)]]) std::uint64_t Weapon_sceneObjectUpdater(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept
{
    HookContext<GlobalContext> hookContext;
    const auto originalReturnValue = hookContext.featuresStates().visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater(weapon, unknown, unknownBool);
    if (auto&& c4 = hookContext.make<BaseWeapon>(weapon).template cast<C4>())
        hookContext.make<ModelGlow>().updateInSceneObjectUpdater()(DroppedBombModelGlow{hookContext}, c4.baseWeapon(), EntityTypeInfo{});
    else
        hookContext.make<ModelGlow>().updateInSceneObjectUpdater()(WeaponModelGlow{hookContext}, hookContext.make<BaseWeapon>(weapon), hookContext.make<BaseWeapon>(weapon).baseEntity().classify());
    return originalReturnValue;
}

float ClientModeHook_getViewmodelFov(cs2::ClientModeCSNormal* clientMode) noexcept
{
    HookContext<GlobalContext> hookContext;
    const auto originalFov = hookContext.hooks().originalGetViewmodelFov(clientMode);
    if (auto&& viewmodelMod = hookContext.template make<ViewmodelMod>(); viewmodelMod.shouldModifyViewmodelFov())
        return viewmodelMod.viewmodelFov();
    return originalFov;
}
