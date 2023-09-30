#pragma once

#include <optional>

#include <CS2/Constants/BombSiteIndex.h>
#include <CS2/Constants/EntityHandle.h>
#include "Implementation/PlantedC4Impl.h"

struct PlantedC4 {
    explicit PlantedC4(cs2::CPlantedC4* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    [[nodiscard]] float getTimeToExplosion(float curtime) const noexcept
    {
        if (ticking())
            return PlantedC4Impl::instance().blowTime.of(thisptr).valueOr(0.0f) - curtime;
        return -1.0f;
    }

    [[nodiscard]] bool isBeingDefused() const noexcept
    {
        return PlantedC4Impl::instance().defuser.of(thisptr).valueOr(cs2::INVALID_EHANDLE_INDEX) != cs2::INVALID_EHANDLE_INDEX;
    }

    [[nodiscard]] std::optional<bool> canBeDefused() const noexcept
    {
        const auto defuseEndTime = PlantedC4Impl::instance().defuseEndTime.of(thisptr);
        const auto blowTime = PlantedC4Impl::instance().blowTime.of(thisptr);
        if (defuseEndTime.get() && blowTime.get())
            return *defuseEndTime.get() < *blowTime.get();
        return {};
    }

    [[nodiscard]] float getTimeToDefuseEnd(float curtime) const noexcept
    {
        return PlantedC4Impl::instance().defuseEndTime.of(thisptr).valueOr(0.0f) - curtime;
    }

    [[nodiscard]] const char* getBombSiteIconUrl() const noexcept
    {
        constexpr auto INVALID_BOMBSITE_INDEX = -1;
        switch (PlantedC4Impl::instance().bombSite.of(thisptr).valueOr(INVALID_BOMBSITE_INDEX)) {
        case cs2::BOMBSITE_A_INDEX: return "s2r://panorama/images/icons/ui/map_bombzone_a.vsvg";
        case cs2::BOMBSITE_B_INDEX: return "s2r://panorama/images/icons/ui/map_bombzone_b.vsvg";
        default: return nullptr;
        }
    }

private:
    [[nodiscard]] bool ticking() const noexcept
    {
        return PlantedC4Impl::instance().ticking.of(thisptr).valueOr(true);
    }

    cs2::CPlantedC4* thisptr;
};
