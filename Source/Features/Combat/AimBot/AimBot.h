#pragma once

#include <cstdint>
#include <limits>

#include <Platform/Macros/IsPlatform.h>
#if IS_WIN64()
#include <Windows.h>
#endif

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Vector.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <HookContext/HookContextMacros.h>
#include "AimBotConfigVariables.h"

template <typename HookContext>
class AimBot {
public:
    explicit AimBot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const
    {
#if IS_WIN64()
        if (!GET_CONFIG_VAR(aim_bot_vars::Enabled) || !activationKeyHeld())
            return;

        auto&& localPawn = hookContext.activeLocalPlayerPawn();
        if (!localPawn)
            return;

        const auto localOrigin = localPawn.absOrigin();
        if (!localOrigin.hasValue())
            return;

        const auto target = findTarget(localPawn, localOrigin.value());
        if (!target.hasTarget)
            return;

        const auto deadzone = static_cast<float>(GET_CONFIG_VAR(aim_bot_vars::DeadzonePixels));
        if (target.screenDistanceSquared <= deadzone * deadzone)
            return;

        moveMouse(target.deltaX, target.deltaY);
#endif
    }

private:
    struct Target {
        bool hasTarget{false};
        float deltaX{};
        float deltaY{};
        float screenDistanceSquared{(std::numeric_limits<float>::max)()};
        float worldDistanceSquared{(std::numeric_limits<float>::max)()};
    };

#if IS_WIN64()
    [[nodiscard]] bool activationKeyHeld() const noexcept
    {
        return (GetAsyncKeyState(GET_CONFIG_VAR(aim_bot_vars::ActivationKey)) & 0x8000) != 0;
    }

    [[nodiscard]] Target findTarget(const auto& localPawn, const cs2::Vector& localOrigin) const noexcept
    {
        Target bestTarget;

        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity(
            [this, &bestTarget, &localPawn, &localOrigin](const auto& entityIdentity) {
                const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(entityIdentity.entityClass);
                if (!entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
                    return;

                auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));
                auto&& targetPawn = baseEntity.template as<PlayerPawn>();
                if (targetPawn.isControlledByLocalPlayer())
                    return;

                if (!targetPawn.isAlive().value_or(false) || !targetPawn.isTTorCT())
                    return;

                if (targetPawn.hasImmunity().valueOr(true))
                    return;

                if (GET_CONFIG_VAR(aim_bot_vars::TeamCheck) && !targetPawn.isEnemy().value_or(false))
                    return;

                const auto targetOrigin = targetPawn.absOrigin();
                if (!targetOrigin.hasValue())
                    return;

                const auto maybeTarget = evaluateTarget(localOrigin, targetOrigin.value());
                if (!maybeTarget.hasTarget)
                    return;

                if (isBetterTarget(maybeTarget, bestTarget))
                    bestTarget = maybeTarget;
            }
        );

        return bestTarget;
    }

    [[nodiscard]] Target evaluateTarget(const cs2::Vector& localOrigin, cs2::Vector targetOrigin) const noexcept
    {
        const auto maxDistance = static_cast<float>(GET_CONFIG_VAR(aim_bot_vars::MaxDistance));
        const auto distanceSquared = localOrigin.squareDistTo(targetOrigin);
        if (distanceSquared > maxDistance * maxDistance)
            return {};

        targetOrigin.z += static_cast<float>(GET_CONFIG_VAR(aim_bot_vars::VerticalAdjustment));

        const auto clipSpace = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(targetOrigin);
        if (!clipSpace.onScreen())
            return {};

        const auto inverseW = 1.0f / clipSpace.w;
        const auto ndcX = clipSpace.x * inverseW;
        const auto ndcY = clipSpace.y * inverseW;
        if (std::abs(ndcX) > 1.0f || std::abs(ndcY) > 1.0f)
            return {};

        const auto screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
        const auto screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
        const auto deltaX = ndcX * screenWidth * 0.5f;
        const auto deltaY = -ndcY * screenHeight * 0.5f;
        const auto screenDistanceSquared = deltaX * deltaX + deltaY * deltaY;
        const auto fov = static_cast<float>(GET_CONFIG_VAR(aim_bot_vars::FovPixels));
        if (screenDistanceSquared > fov * fov)
            return {};

        return Target{
            .hasTarget = true,
            .deltaX = deltaX,
            .deltaY = deltaY,
            .screenDistanceSquared = screenDistanceSquared,
            .worldDistanceSquared = distanceSquared
        };
    }

    [[nodiscard]] static bool isBetterTarget(const Target& candidate, const Target& current) noexcept
    {
        if (!current.hasTarget)
            return true;
        if (candidate.screenDistanceSquared != current.screenDistanceSquared)
            return candidate.screenDistanceSquared < current.screenDistanceSquared;
        return candidate.worldDistanceSquared < current.worldDistanceSquared;
    }

    void moveMouse(float deltaX, float deltaY) const noexcept
    {
        const auto smooth = static_cast<float>(GET_CONFIG_VAR(aim_bot_vars::Smooth));
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.dx = smoothDelta(deltaX / smooth);
        input.mi.dy = smoothDelta(deltaY / smooth);
        if (input.mi.dx != 0 || input.mi.dy != 0)
            SendInput(1, &input, sizeof(INPUT));
    }

    [[nodiscard]] static LONG smoothDelta(float delta) noexcept
    {
        if (delta > 0.0f)
            return (std::max)(1L, static_cast<LONG>(delta));
        if (delta < 0.0f)
            return (std::min)(-1L, static_cast<LONG>(delta));
        return 0;
    }
#endif

    HookContext& hookContext;
};
