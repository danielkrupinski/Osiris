#pragma once

#include "DeathNotice.h"

template <typename Panel, typename Dependencies>
struct DeathNotices {
    DeathNotices(Panel panel, Dependencies dependencies) noexcept
        : panel{panel}
        , dependencies{dependencies}
    {
    }

    template <typename F>
    void forEach(F&& f) noexcept
    {
        panel.children().forEach([&f, this](const auto& panel) { f(DeathNotice<PanoramaUiPanel, Dependencies&>{PanoramaUiPanel{panel}, dependencies}); });
    }

    Panel panel;
    Dependencies dependencies;
};
