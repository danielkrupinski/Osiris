#pragma once

#include <optional>
#include <GameClient/Entities/PlayerPawn.h>
#include <HookContext/HookContextMacros.h>
#include "ViewmodelModConfigVariables.h"

template <typename HookContext>
class ViewmodelMod {
public:
    explicit ViewmodelMod(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool fovModificationActive() const
    {
        return GET_CONFIG_VAR(viewmodel_mod_vars::Enabled) && GET_CONFIG_VAR(viewmodel_mod_vars::ModifyFov);
    }

    [[nodiscard]] bool shouldModifyViewmodelFov() const
    {
        auto&& localPlayerPawn = hookContext.localPlayerController().pawn().template cast<PlayerPawn>();
        return localPlayerPawn && !localPlayerPawn.isScoped().valueOr(false);
    }

    [[nodiscard]] float viewmodelFov() const
    {
        return GET_CONFIG_VAR(viewmodel_mod_vars::Fov);
    }

private:
    HookContext& hookContext;
};
