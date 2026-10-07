#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/IsPlatform.h>

#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>

template <typename HookContext>
class Radar {
public:
    explicit Radar(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
#if IS_WIN64()
        if (!GET_CONFIG_VAR(player_info_vars::RadarEnabled)
            || !playerPawn
            || playerPawn.isControlledByLocalPlayer()
            || !playerPawn.isTTorCT()
            || !playerPawn.isAlive().value_or(false)
            || !playerPawn.isEnemy().value_or(false))
            return;

        // The offset is from the current Windows client schema. Keep this
        // feature disabled by default because schema offsets can change after
        // a game update.
        constexpr std::ptrdiff_t entitySpottedStateOffset{0x1E88};
        constexpr std::ptrdiff_t spottedFlagOffset{0x08};
        auto* spottedFlag = reinterpret_cast<bool*>(
            reinterpret_cast<std::byte*>(playerPawn.raw()) + entitySpottedStateOffset + spottedFlagOffset);
        *spottedFlag = true;
#else
        (void)playerPawn;
#endif
    }

private:
    HookContext& hookContext;
};
