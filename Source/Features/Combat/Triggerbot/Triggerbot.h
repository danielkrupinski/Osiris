#pragma once

#include <cstddef>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Vector.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/IsPlatform.h>

#include "TriggerbotConfigVariables.h"
#include "TriggerbotState.h"

#if IS_WIN64()
#include <Windows.h>
#endif

template <typename HookContext>
class Triggerbot {
public:
    explicit Triggerbot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(triggerbot_vars::Enabled)}
    {
    }

    void checkPlayer(auto&& playerPawn) noexcept
    {
        if (!enabled || targetFound || !isGameInForeground() || !isLocalPlayerInGame())
            return;
        if (!playerPawn.isAlive().value_or(true))
            return;
        if (playerPawn.isControlledByLocalPlayer())
            return;
        if (!playerPawn.isTTorCT())
            return;
        if (!playerPawn.isEnemy().value_or(true))
            return;

        const auto origin = playerPawn.absOrigin();
        if (!origin.hasValue())
            return;
        const auto& org = origin.value();

        if (GET_CONFIG_VAR(triggerbot_vars::VisibilityCheck)) {
            if (playerPawn.isDormant())
                return;

            const auto localPawn = getLocalPlayerPawn();
            if (!localPawn)
                return;
            const auto eyePos = getEyePosition(localPawn);
            const auto chestZ = org.z + kChestZOffset;
            const auto dx = org.x - eyePos.x;
            const auto dy = org.y - eyePos.y;
            const auto dz = chestZ - eyePos.z;
            if (dx * dx + dy * dy + dz * dz > kMaxEngagementRangeSq)
                return;
        }

        // Multi-point body check: at face-to-face range the head is angularly
        // way above the crosshair while the waist/pelvis is much closer to it.
        // Any body point within the tight radius means "crosshair on target".
        const auto converter = hookContext.template make<WorldToClipSpaceConverter>();
        const auto ndcRadius = calculateNdcRadius();
        const auto ndcRadiusSq = ndcRadius * ndcRadius;
        for (const auto zOff : kBodyZOffsets) {
            const cs2::Vector p{org.x, org.y, org.z + zOff};
            const auto clip = converter.toClipSpace(p);
            if (!clip.onScreen())
                continue;
            const auto ndcX = clip.x / clip.w;
            const auto ndcY = clip.y / clip.w;
            if (ndcX * ndcX + ndcY * ndcY < ndcRadiusSq) {
                targetFound = true;
                return;
            }
        }
    }

    void execute() noexcept
    {
        removeFlash();

        if (!enabled || !isLocalPlayerAlive() || !isGameInForeground()) {
            targetFound = false;
            return;
        }

        if (targetFound)
            fire();

        targetFound = false;
    }

private:
    void removeFlash() noexcept
    {
        if (!GET_CONFIG_VAR(triggerbot_vars::FlashImmunity))
            return;

        const auto localPawn = getLocalPlayerPawn();
        if (!localPawn)
            return;

        const auto flashEndTime = hookContext.patternSearchResults()
            .template get<OffsetToFlashBangEndTime>().of(localPawn).get();
        if (flashEndTime && *flashEndTime > 0.0f)
            *const_cast<float*>(flashEndTime) = 0.0f;
    }

    [[nodiscard]] cs2::C_CSPlayerPawn* getLocalPlayerPawn() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (!localController)
            return nullptr;
        const auto handle = localController.playerPawnHandle();
        if (!handle)
            return nullptr;
        return static_cast<cs2::C_CSPlayerPawn*>(
            hookContext.template make<EntitySystem>().getEntityFromHandle(*handle));
    }

    [[nodiscard]] static cs2::Vector getEyePosition(cs2::C_CSPlayerPawn* pawn) noexcept
    {
        const auto sceneNode = *reinterpret_cast<std::byte**>(
            reinterpret_cast<std::byte*>(pawn) + kGameSceneNodeOffset);
        if (!sceneNode)
            return {};
        auto origin = *reinterpret_cast<cs2::Vector*>(sceneNode + kAbsOriginOffset);
        origin.z += kEyeHeightOffset;
        return origin;
    }

    void fire() noexcept
    {
#if IS_WIN64()
        INPUT inputs[2]{};
        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(2, inputs, sizeof(INPUT));
#endif
    }

    [[nodiscard]] bool isGameInForeground() const noexcept
    {
#if IS_WIN64()
        const auto fgWindow = GetForegroundWindow();
        if (!fgWindow)
            return false;
        DWORD fgProcessId = 0;
        GetWindowThreadProcessId(fgWindow, &fgProcessId);
        return fgProcessId == GetCurrentProcessId();
#else
        return true;
#endif
    }

    [[nodiscard]] bool isLocalPlayerInGame() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        return static_cast<bool>(localController);
    }

    [[nodiscard]] bool isLocalPlayerAlive() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (!localController)
            return false;
        const auto isAlive = localController.isPawnAlive();
        return isAlive.value_or(false);
    }

    [[nodiscard]] static float calculateNdcRadius() noexcept
    {
#if IS_WIN64()
        const auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
        if (screenWidth <= 0)
            return 0.05f;
        // Tight 60-pixel radius for pure triggerbot semantics — only fire when
        // the crosshair is essentially on top of the enemy.
        constexpr auto kTargetPixelRadius = 60.0f;
        return (2.0f * kTargetPixelRadius) / static_cast<float>(screenWidth);
#else
        return 0.05f;
#endif
    }

    [[nodiscard]] TriggerbotState& state() const noexcept
    {
        return hookContext.featuresStates().triggerbotState;
    }

    static constexpr std::ptrdiff_t kGameSceneNodeOffset{0x330};
    static constexpr std::ptrdiff_t kAbsOriginOffset{0xC8};
    static constexpr float kMaxEngagementRangeSq{3500.0f * 3500.0f};
    static constexpr float kEyeHeightOffset{64.0f};
    static constexpr float kChestZOffset{50.0f};
    // Body sample points (z offset above absOrigin). Order doesn't matter — we
    // fire as soon as any one of them is inside the crosshair radius.
    static constexpr float kBodyZOffsets[4]{64.0f, 50.0f, 36.0f, 20.0f};

    HookContext& hookContext;
    bool enabled;
    bool targetFound{false};
};
