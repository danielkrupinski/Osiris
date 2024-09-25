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
    explicit PlantedC4(PlantedC4Base base, Dependencies& dependencies) noexcept
        : base{base}
        , dependencies{dependencies}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return dependencies.template make<BaseEntity>(base.thisptr);
    }

    [[nodiscard]] auto getTimeToExplosion() const noexcept
    {
        return base.blowTime().toOptional() - dependencies.globalVars().curtime();
    }

    [[nodiscard]] auto isTicking() const noexcept
    {
        return base.ticking().toOptional();
    }

    [[nodiscard]] bool isBeingDefused() const noexcept
    {
        return base.defuser().valueOr(cs2::INVALID_EHANDLE_INDEX) != cs2::INVALID_EHANDLE_INDEX;
    }

    [[nodiscard]] auto canBeDefused() const noexcept
    {
        return base.defuseEndTime().toOptional().lessThan(base.blowTime().toOptional());
    }

    [[nodiscard]] auto getTimeToDefuseEnd() const noexcept
    {
        return base.defuseEndTime().toOptional() - dependencies.globalVars().curtime();
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
    Dependencies& dependencies;
};
