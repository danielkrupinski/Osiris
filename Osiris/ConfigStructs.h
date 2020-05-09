#pragma once

struct PurchaseList {
    bool enabled = false;
    bool onlyDuringFreezeTime = false;
    bool showPrices = false;
    bool noTitleBar = false;

    enum Mode {
        Details = 0,
        Summary
    };
    int mode = Details;

    auto operator==(const PurchaseList& pl) const
    {
        return enabled == pl.enabled
            && onlyDuringFreezeTime == pl.onlyDuringFreezeTime
            && showPrices == pl.showPrices
            && noTitleBar == pl.noTitleBar
            && mode == pl.mode;
    }
};
