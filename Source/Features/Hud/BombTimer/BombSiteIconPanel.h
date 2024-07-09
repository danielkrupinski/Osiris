#pragma once

template <typename ImagePanel>
struct BombSiteIconPanel {
    explicit BombSiteIconPanel(ImagePanel panel) noexcept
        : panel{panel}
    {
    }

    void setIcon(const char* bombsiteIconUrl) const noexcept
    {
        if (bombsiteIconUrl)
            panel.setImageSvg(bombsiteIconUrl);
    }

    ImagePanel panel;
};
