#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <Utils/Optional.h>

template <typename HookContext>
class PlayerControllerForModelGlowPreview {
public:
    explicit PlayerControllerForModelGlowPreview(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] Optional<cs2::CCSPlayerController::m_iCompTeammateColor> playerColorIndex() const noexcept
    {
        return hookContext.playerModelGlowPreviewState().previewPlayerColorIndex;
    }

private:
    HookContext& hookContext;
};
