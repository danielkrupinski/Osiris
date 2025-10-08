#pragma once

#include <optional>
#include <HookContext/HookContextMacros.h>
#include "ViewmodelModConfigVariables.h"

template <typename HookContext>
class ViewmodelMod {
public:
    explicit ViewmodelMod(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] std::optional<float> viewmodelFov() const
    {
        if (shouldModifyViewmodelFov())
            return GET_CONFIG_VAR(viewmodel_mod_vars::Fov);
        return {};
    }

private:
    [[nodiscard]] bool shouldModifyViewmodelFov() const
    {
        auto&& localPlayerPawn = hookContext.localPlayerController().pawn().template cast<PlayerPawn>();
        return localPlayerPawn && !localPlayerPawn.isScoped().valueOr(false);
    }

    HookContext& hookContext;
};
