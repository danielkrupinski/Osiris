#pragma once

#include <HookContext/HookContextMacros.h>
#include "ThirdPersonConfigVariables.h"

// Third-person camera via ClientModeCSNormal::OverrideView hook.
// vtable index 15 verified via ClientModeCSNormal RTTI/vftable lookup.
// Camera moved behind the player in EntryPoints::ClientModeHook_overrideView.
template <typename HookContext>
class ThirdPerson {
public:
    explicit ThirdPerson(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(thirdperson_vars::Enabled)}
    {
    }

    void run() noexcept
    {
        // Camera logic lives in EntryPoints.h OverrideView hook.
        // This feature just provides the config toggle.
    }

private:
    HookContext& hookContext;
    bool enabled;
};
