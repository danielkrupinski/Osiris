#pragma once

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Utils/ReturnAddress.h"
#include <cstddef>
#include <cstdint>
#include <Features/Combat/Aimbot/SilentAim.h>
#include <CS2/Classes/CSGOInput.h>
#include <CS2/Offsets/ClientDllOffsets.h>
#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()
#include <Windows.h>
#endif

namespace {
[[nodiscard]] bool isReadableMemory(const void* address, std::size_t size) noexcept;
[[nodiscard]] bool isExecutableMemory(const void* address) noexcept;

[[nodiscard]] bool hasReadableVmt(cs2::CCSGOInput* input) noexcept
{
    if (!isReadableMemory(input, sizeof(void*)))
        return false;

    auto* const vmt = *reinterpret_cast<void***>(input);
    if (!isReadableMemory(vmt, sizeof(void*)))
        return false;

    return isExecutableMemory(vmt[0]);
}

[[nodiscard]] cs2::CCSGOInput* csgoInputInstance() noexcept
{
#if IS_WIN64()
    const auto clientDll = GetModuleHandleA("client.dll");
    if (!clientDll)
        return nullptr;

    auto* inputAddress = reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kCSGOInput;
    auto* directInput = reinterpret_cast<cs2::CCSGOInput*>(inputAddress);
    if (hasReadableVmt(directInput))
        return directInput;

    if (!isReadableMemory(inputAddress, sizeof(cs2::CCSGOInput*)))
        return nullptr;

    auto* indirectInput = *reinterpret_cast<cs2::CCSGOInput**>(inputAddress);
    if (hasReadableVmt(indirectInput))
        return indirectInput;

    return nullptr;
#else
    return nullptr;
#endif
}

class ScopedGlobalViewAnglesOverride {
public:
    ScopedGlobalViewAnglesOverride(bool enabled, float pitch, float yaw) noexcept
    {
#if IS_WIN64()
        if (!enabled)
            return;
        const auto clientDll = GetModuleHandleA("client.dll");
        if (!clientDll)
            return;
        angles = reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kViewAngles);
        if (!isReadableMemory(angles, sizeof(previous)))
            return;

        previous[0] = angles[0];
        previous[1] = angles[1];
        previous[2] = angles[2];
        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0.0f;
        active = true;
#else
        (void)enabled; (void)pitch; (void)yaw;
#endif
    }

    ~ScopedGlobalViewAnglesOverride()
    {
#if IS_WIN64()
        if (!active || !angles)
            return;
        angles[0] = previous[0];
        angles[1] = previous[1];
        angles[2] = previous[2];
#endif
    }

private:
    float* angles{nullptr};
    float previous[3]{};
    bool active{false};
};
}

[[NOINLINE]] void finishInit(auto& hookContext)
{
    hookContext.entityClassifier().init(hookContext);
    if (const auto mainMenu{hookContext.patternSearchResults().template get<MainMenuPanelPointer>()}; mainMenu && *mainMenu)
        hookContext.template make<PanoramaGUI>().init(hookContext.template make<PanoramaUiPanel>((*mainMenu)->uiPanel));
    hookContext.config().init();
    hookContext.config().scheduleLoad();
    hookContext.hooks().createMoveHook.install(csgoInputInstance());
    hookContext.template make<ClientModeHooks>().hookGetViewmodelFov();
    hookContext.template make<ClientModeHooks>().hookOverrideView();
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
    hookContext.template make<ClientModeHooks>().restoreAll();
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

// CreateMove hook: runs inside CSGOInput::CreateMove BEFORE the user command
// is sent to the server. When silent aim is active (set by Aimbot in the
// previous render frame), we replace the CUserCmd.viewangles so the bullet
// goes to the head WITHOUT moving the visible crosshair. This is true silent
// aim — the server sees a headshot while the client shows the user's real
// aim direction.
bool CreateMoveHook_onCreateMove(cs2::CCSGOInput* thisptr, int slot, cs2::CUserCmd* cmd) noexcept
{
    if (!HookContext<GlobalContext>::isGlobalContextComplete())
        return false;
    HookContext<GlobalContext> hookContext;
    auto& hooks = hookContext.hooks();
    const bool applySilentAim = hooks.silentAimActive;
    bool result = false;
    {
        ScopedGlobalViewAnglesOverride viewAnglesOverride{
            applySilentAim, hooks.silentAimPitch, hooks.silentAimYaw};
        if (auto* orig = hooks.createMoveHook.originalCreateMove())
            result = orig(thisptr, slot, cmd);
    }
    if (!applySilentAim)
        return result;
    if (cmd && isReadableMemory(cmd, sizeof(cs2::CUserCmd))) {
        static_cast<void>(aimbot::silent::applyToUserCmd(
            *cmd, hooks.silentAimPitch, hooks.silentAimYaw, isReadableMemory));
    }
    hooks.silentAimActive = false; // consumed, don't apply to next tick
    return result;
}

namespace {

[[nodiscard]] bool isReadableMemory(const void* address, std::size_t size) noexcept
{
#if IS_WIN64()
    if (!address || size == 0)
        return false;

    MEMORY_BASIC_INFORMATION info{};
    if (!VirtualQuery(address, &info, sizeof(info)))
        return false;
    if (info.State != MEM_COMMIT)
        return false;
    if ((info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0)
        return false;

    const auto start = reinterpret_cast<std::uintptr_t>(address);
    const auto end = start + size;
    const auto regionEnd = reinterpret_cast<std::uintptr_t>(info.BaseAddress) + info.RegionSize;
    return end > start && end <= regionEnd;
#else
    return address != nullptr;
#endif
}

[[nodiscard]] bool isExecutableMemory(const void* address) noexcept
{
#if IS_WIN64()
    if (!address)
        return false;

    MEMORY_BASIC_INFORMATION info{};
    if (!VirtualQuery(address, &info, sizeof(info)))
        return false;
    if (info.State != MEM_COMMIT)
        return false;
    if ((info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0)
        return false;

    constexpr DWORD kExecutablePages =
        PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    return (info.Protect & kExecutablePages) != 0;
#else
    return address != nullptr;
#endif
}

[[nodiscard]] float clampFloat(float value, float min, float max) noexcept
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

[[nodiscard]] float normalizeYaw(float yaw) noexcept
{
    while (yaw > 180.0f)
        yaw -= 360.0f;
    while (yaw < -180.0f)
        yaw += 360.0f;
    return yaw;
}

[[nodiscard]] bool isReasonableAngle(float angle) noexcept
{
    return angle > -10000.0f && angle < 10000.0f;
}

[[nodiscard]] float fastFabs(float x) noexcept
{
    return x < 0.0f ? -x : x;
}

[[nodiscard]] float fastSqrt(float x) noexcept
{
    if (x <= 0.0f)
        return 0.0f;
    std::int32_t i;
    const float x2 = x * 0.5f;
    auto y = x;
    i = *reinterpret_cast<std::int32_t*>(&y);
    i = 0x5f3759df - (i >> 1);
    y = *reinterpret_cast<float*>(&i);
    y = y * (1.5f - (x2 * y * y));
    return 1.0f / y;
}

[[nodiscard]] float fastAtan2(float y, float x) noexcept
{
    const auto ax = fastFabs(x);
    const auto ay = fastFabs(y);
    if (ax < 0.000001f && ay < 0.000001f)
        return 0.0f;
    const auto a = (ax < ay) ? (ax / ay) : (ay / ax);
    const auto s = a * a;
    auto r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;
    if (ay > ax)
        r = 1.57079637f - r;
    if (x < 0.0f)
        r = 3.14159274f - r;
    if (y < 0.0f)
        r = -r;
    return r;
}

// CRT-free trig for OverrideView (project links /nodefaultlib).
[[nodiscard]] inline float fastSin(float x) noexcept {
    constexpr float kPi = 3.14159265f;
    constexpr float kTwoPi = 6.28318530f;
    constexpr float kHalfPi = 1.57079633f;
    while (x > kPi)
        x -= kTwoPi;
    while (x < -kPi)
        x += kTwoPi;
    if (x > kHalfPi)
        x = kPi - x;
    else if (x < -kHalfPi)
        x = -kPi - x;

    const float x2 = x * x;
    return x * (1.0f - x2 * (0.16605f - x2 * 0.00761f));
}

[[nodiscard]] inline float fastCos(float x) noexcept {
    return fastSin(x + 1.57079633f);
}

cs2::C_CSPlayerPawn* getReadableLocalPawn(HookContext<GlobalContext>& hookContext) noexcept
{
    constexpr std::size_t kControllerReadableSize{0x910};
    constexpr std::size_t kPawnReadableSize{0x1228};

    auto&& localController = hookContext.localPlayerController();
    if (!localController || !isReadableMemory(localController.rawPointer(), kControllerReadableSize))
        return nullptr;

    const auto handle = localController.playerPawnHandle();
    if (!handle || !handle->index().isValid())
        return nullptr;

    auto* pawn = static_cast<cs2::C_CSPlayerPawn*>(
        hookContext.template make<EntitySystem>().getEntityFromHandle(*handle));
    if (!isReadableMemory(pawn, kPawnReadableSize))
        return nullptr;

    return pawn;
}

}

// OverrideView hook — enables third-person camera by moving the view
// origin behind the player before CS2 renders the frame.
// Vtable index 15 on ClientModeCSNormal, verified via RTTI/vftable lookup:
// .?AVClientModeCSNormal@@ -> vftable RVA 0x1AEF040 -> entry 15 RVA 0xC63230
// Vtable bounds are validated at install time; index 15 is unchanged on build 14171.
void ClientModeHook_overrideView(cs2::ClientModeCSNormal* thisptr, cs2::CViewSetup* setup) noexcept
{
    if (!HookContext<GlobalContext>::isGlobalContextComplete())
        return;
    if (!setup || !isReadableMemory(setup, sizeof(cs2::CViewSetup)))
        return;

    HookContext<GlobalContext> hookContext;

    if (auto* orig = hookContext.hooks().originalOverrideView)
        orig(thisptr, setup);

    if (!GET_CONFIG_VAR(thirdperson_vars::Enabled))
        return;

    auto* pawn = getReadableLocalPawn(hookContext);
    if (!pawn)
        return;

    // Use the same pattern-backed origin accessor as upstream features.
    // This avoids pinning the camera to C_BaseEntity/CGameSceneNode offsets.
    const auto originValue = hookContext.make<PlayerPawn>(pawn).absOrigin();
    if (!originValue.hasValue())
        return;
    const auto origin = originValue.value();

    // Prefer the view angles already prepared by the game/original OverrideView.
    float pitch = setup->m_angView[0];
    float yaw = setup->m_angView[1];
    if (!isReasonableAngle(pitch) || !isReasonableAngle(yaw)) {
#if IS_WIN64()
        const auto clientDll = GetModuleHandleA("client.dll");
        if (!clientDll) return;
        const auto* angles = reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kViewAngles);
        pitch = angles[0];
        yaw = angles[1];
#endif
        if (!isReasonableAngle(pitch) || !isReasonableAngle(yaw))
            return;
    }

#if !IS_WIN64()
    pitch = 0.0f;
    yaw = 0.0f;
#endif
    pitch = clampFloat(pitch, -89.0f, 89.0f);
    yaw = normalizeYaw(yaw);

    // Reference-style follow camera: mostly behind the player, slightly low,
    // and aimed at the upper torso / forward path. A large side offset pushes
    // the local model into a screen corner, so keep the lateral offset neutral.
    constexpr float kCameraDistance = 96.0f;
    constexpr float kCameraSideOffset = 0.0f;
    constexpr float kCameraHeight = 66.0f;
    constexpr float kLookAhead = 70.0f;
    constexpr float kLookTargetHeight = 58.0f;
    constexpr float kDegToRad = 0.01745329252f;
    constexpr float kRadToDeg = 57.295779513082f;
    const float yr = yaw * kDegToRad;
    const float cy = fastCos(yr), sy = fastSin(yr);
    const float forwardX = cy;
    const float forwardY = sy;
    const float rightX = -sy;
    const float rightY = cy;
    const float camX = origin.x - forwardX * kCameraDistance + rightX * kCameraSideOffset;
    const float camY = origin.y - forwardY * kCameraDistance + rightY * kCameraSideOffset;
    const float camZ = origin.z + kCameraHeight;
    const float targetX = origin.x + forwardX * kLookAhead;
    const float targetY = origin.y + forwardY * kLookAhead;
    const float targetZ = origin.z + kLookTargetHeight + clampFloat(-pitch * 0.10f, -8.0f, 8.0f);
    const float deltaX = targetX - camX;
    const float deltaY = targetY - camY;
    const float deltaZ = targetZ - camZ;
    const float horizontal = fastSqrt(deltaX * deltaX + deltaY * deltaY);
    const float cameraYaw = normalizeYaw(fastAtan2(deltaY, deltaX) * kRadToDeg);
    const float cameraPitch = clampFloat(-fastAtan2(deltaZ, horizontal) * kRadToDeg, -28.0f, 22.0f);

    setup->m_vecOrigin[0] = camX;
    setup->m_vecOrigin[1] = camY;
    setup->m_vecOrigin[2] = camZ;
    setup->m_angView[0] = cameraPitch;
    setup->m_angView[1] = cameraYaw;
    setup->m_angView[2] = 0.0f;
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
