#pragma once

template <typename ImagePanel>
struct BombSiteIconPanel {
    void setIcon(const char* bombsiteIconUrl) const noexcept
    {
        if (bombsiteIconUrl)
            panel.setImageSvg(bombsiteIconUrl);
    }

    ImagePanel panel;
};
