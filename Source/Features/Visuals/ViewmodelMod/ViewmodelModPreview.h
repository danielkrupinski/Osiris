#pragma once

#include <HookContext/HookContextMacros.h>
#include "ViewmodelModConfigVariables.h"

template <typename HookContext>
class ViewmodelModPreview {
public:
    explicit ViewmodelModPreview(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] float viewmodelFovForPreview() const
    {
        if (fovModificationActive())
            return GET_CONFIG_VAR(viewmodel_mod_vars::Fov);
        return viewmodelFovFromConVar();
    }

private:
    [[nodiscard]] bool fovModificationActive() const
    {
        return GET_CONFIG_VAR(viewmodel_mod_vars::Enabled) && GET_CONFIG_VAR(viewmodel_mod_vars::ModifyFov);
    }

    [[nodiscard]] float viewmodelFovFromConVar() const
    {
        return GET_CONVAR_VALUE(cs2::viewmodel_fov).value_or(viewmodel_mod_params::kFallbackFov);
    }

    HookContext& hookContext;
};
