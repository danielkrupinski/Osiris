#pragma once

#include <Platform/Macros/IsPlatform.h>
#if IS_WIN64()
#include <Windows.h>
#endif

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <HookContext/HookContextMacros.h>
#include "TriggerBotConfigVariables.h"
#include "TriggerBotState.h"

template <typename HookContext>
class TriggerBot {
public:
    TriggerBot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const
    {
        if (!GET_CONFIG_VAR(trigger_bot_vars::Enabled)) {
            state().phase = TriggerBotState::Phase::Idle;
            return;
        }

        const auto curtime = hookContext.globalVars().curtime();
        if (!curtime.hasValue())
            return;

        switch (state().phase) {
        case TriggerBotState::Phase::Idle:
            if (shouldFire()) {
                state().phase = TriggerBotState::Phase::Pending;
                state().phaseStartTime = curtime.value();
            }
            break;

        case TriggerBotState::Phase::Pending: {
            const auto delayMin = GET_CONFIG_VAR(trigger_bot_vars::DelayMin);
            const auto delayMax = GET_CONFIG_VAR(trigger_bot_vars::DelayMax);
            const auto delay = static_cast<float>(delayMin + (delayMax - delayMin) / 2) / 1000.0f;
            if (curtime.value() - state().phaseStartTime >= delay) {
                if (shouldFire())
                    fireWeapon();
                state().phase = TriggerBotState::Phase::Sleep;
                state().phaseStartTime = curtime.value();
            }
            break;
        }

        case TriggerBotState::Phase::Sleep: {
            const auto duration = static_cast<float>(GET_CONFIG_VAR(trigger_bot_vars::ShotDuration)) / 1000.0f;
            if (curtime.value() - state().phaseStartTime >= duration)
                state().phase = TriggerBotState::Phase::Idle;
            break;
        }
        }
    }

private:
    [[nodiscard]] bool shouldFire() const noexcept
    {
        auto&& localPawn = hookContext.activeLocalPlayerPawn();
        if (!localPawn)
            return false;

        const auto crosshairEntityIndex = localPawn.crosshairEntityIndex();
        if (!crosshairEntityIndex.hasValue())
            return false;

        auto&& baseEntity = hookContext.template make<EntitySystem>().getEntityFromIndex2(crosshairEntityIndex.value());
        if (!baseEntity.template is<PlayerPawn>())
            return false;

        auto&& targetPawn = baseEntity.template cast<PlayerPawn>();

        if (!targetPawn.isAlive().value_or(false))
            return false;

        if (GET_CONFIG_VAR(trigger_bot_vars::TeamCheck) && !targetPawn.isEnemy().value_or(true))
            return false;

        return true;
    }

    void fireWeapon() const noexcept
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

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().triggerBotState;
    }

    HookContext& hookContext;
};
