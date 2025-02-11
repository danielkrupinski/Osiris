#pragma once

#include <CS2/Classes/Entities/C_CSGO_PreviewPlayer.h>
#include "BaseEntity.h"

template <typename HookContext>
class PreviewPlayer {
public:
    using RawType = cs2::C_CSGO_PreviewPlayer;

    PreviewPlayer(HookContext& hookContext, cs2::C_CSGO_PreviewPlayer* previewPlayer) noexcept
        : hookContext{hookContext}
        , previewPlayer{previewPlayer}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(previewPlayer);
    }

    [[nodiscard]] operator cs2::C_CSGO_PreviewPlayer*() const noexcept
    {
        return previewPlayer;
    }

private:
    HookContext& hookContext;
    cs2::C_CSGO_PreviewPlayer* previewPlayer;
};
