#pragma once

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <MemoryPatterns/PatternTypes/PlayerPawnPatternTypes.h>

template <typename HookContext>
class PlayerMovementServices {
public:
    PlayerMovementServices(HookContext& hookContext, cs2::C_CSPlayerPawn* playerPawn) noexcept
        : hookContext{hookContext}
        , movementServices{static_cast<cs2::CCSPlayer_MovementServices*>(hookContext.patternSearchResults().template get<OffsetToMovementServices>().of(playerPawn).valueOr(nullptr))}
    {
    }

    [[nodiscard]] auto stashGrenadeParameterWhen() const noexcept { return serviceField<OffsetToStashGrenadeParameterWhen>(); }
    [[nodiscard]] auto grenadeParametersStashed() const noexcept { return serviceField<OffsetToGrenadeParametersStashed>(); }
    [[nodiscard]] auto stashedShootAngles() const noexcept { return serviceField<OffsetToStashedShootAngles>(); }
    [[nodiscard]] auto stashedGrenadeThrowPosition() const noexcept { return serviceField<OffsetToStashedGrenadeThrowPosition>(); }
    [[nodiscard]] auto stashedVelocity() const noexcept { return serviceField<OffsetToStashedVelocity>(); }

private:
    template <typename Offset>
    [[nodiscard]] auto serviceField() const noexcept
    {
        return hookContext.patternSearchResults().template get<Offset>().of(movementServices).toOptional();
    }

    HookContext& hookContext;
    cs2::CCSPlayer_MovementServices* movementServices;
};
