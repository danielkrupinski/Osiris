#pragma once

#include <optional>

#include <CS2/Constants/BombSiteIndex.h>
#include <CS2/Constants/EntityHandle.h>
#include <CS2/Constants/IconURLs.h>
#include <GameDependencies/PlantedC4Deps.h>
#include <GameClasses/GlobalVars.h>

struct PlantedC4Base {
    [[nodiscard]] auto blowTime() const noexcept
    {
        return PlantedC4Deps::instance().blowTime.of(thisptr);
    }

    [[nodiscard]] auto defuser() const noexcept
    {
        return PlantedC4Deps::instance().defuser.of(thisptr);
    }

    [[nodiscard]] auto defuseEndTime() const noexcept
    {
        return PlantedC4Deps::instance().defuseEndTime.of(thisptr);
    }

    [[nodiscard]] auto bombSite() const noexcept
    {
        return PlantedC4Deps::instance().bombSite.of(thisptr);
    }

    [[nodiscard]] auto ticking() const noexcept
    {
        return PlantedC4Deps::instance().ticking.of(thisptr);
    }

    cs2::CPlantedC4* thisptr;
};

template <typename Dependencies>
struct PlantedC4 {
    explicit PlantedC4(PlantedC4Base base, Dependencies dependencies) noexcept
        : base{base}
        , dependencies{dependencies}
    {
    }

    [[nodiscard]] float getTimeToExplosion() const noexcept
    {
        if (const auto curtime = dependencies.globalVars().curtime())
            return base.blowTime().valueOr(0.0f) - *curtime;
        return -1.0f;
    }

    [[nodiscard]] bool isTicking() const noexcept
    {
        return base.ticking().valueOr(true) && getTimeToExplosion() > 0.0f;
    }

    [[nodiscard]] bool isBeingDefused() const noexcept
    {
        return base.defuser().valueOr(cs2::INVALID_EHANDLE_INDEX) != cs2::INVALID_EHANDLE_INDEX;
    }

    [[nodiscard]] std::optional<bool> canBeDefused() const noexcept
    {
        const auto defuseEndTime = base.defuseEndTime();
        const auto blowTime = base.blowTime();
        if (defuseEndTime.get() && blowTime.get())
            return *defuseEndTime.get() < *blowTime.get();
        return {};
    }

    [[nodiscard]] std::optional<float> getTimeToDefuseEnd() const noexcept
    {
        if (const auto curtime = dependencies.globalVars().curtime())
            return base.defuseEndTime().valueOr(0.0f) - *curtime;
        return {};
    }

    [[nodiscard]] const char* getBombSiteIconUrl() const noexcept
    {
        constexpr auto INVALID_BOMBSITE_INDEX = -1;
        switch (base.bombSite().valueOr(INVALID_BOMBSITE_INDEX)) {
        case cs2::BOMBSITE_A_INDEX: return cs2::kBombSiteAIconUrl;
        case cs2::BOMBSITE_B_INDEX: return cs2::kBombSiteBIconUrl;
        default: return nullptr;
        }
    }

private:
    PlantedC4Base base;
    Dependencies dependencies;
};
