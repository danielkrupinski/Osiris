#pragma once

#include <array>
#include <cstddef>

#include <CS2/Panorama/PanelHandle.h>

struct SpectatorListState {
    static constexpr std::size_t kMaxSpectators{64};

    cs2::PanelHandle containerPanelHandle{};
    cs2::PanelHandle headerPanelHandle{};
    std::array<cs2::PanelHandle, kMaxSpectators> namePanelHandles{};
};
