#pragma once

#include <optional>

#include <CS2/Constants/BombSiteIndex.h>
#include <CS2/Constants/EntityHandle.h>
#include <CS2/Constants/IconURLs.h>
#include <GameDependencies/PlantedC4Deps.h>
#include <GameClasses/GlobalVars.h>

template <typename HookContext>
class PlantedC4Context {
public:
    PlantedC4Context(HookContext& hookContext, cs2::CPlantedC4* plantedC4) noexcept
        : hookContext{hookContext}
        , plantedC4{plantedC4}
    {
    }

    [[nodiscard]] auto blowTime() const noexcept
    {
        return deps().blowTime.of(plantedC4);
    }

    [[nodiscard]] auto defuser() const noexcept
    {
        return deps().defuser.of(plantedC4);
    }

    [[nodiscard]] auto defuseEndTime() const noexcept
    {
        return deps().defuseEndTime.of(plantedC4);
    }

    [[nodiscard]] auto bombSite() const noexcept
    {
        return deps().bombSite.of(plantedC4);
    }

    [[nodiscard]] auto ticking() const noexcept
    {
        return deps().ticking.of(plantedC4);
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(plantedC4);
    }

    [[nodiscard]] decltype(auto) curtime() const noexcept
    {
        return hookContext.globalVars().curtime();
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().plantedC4Deps;
    }

    HookContext& hookContext;
    cs2::CPlantedC4* plantedC4;
};

template <typename HookContext, typename Context = PlantedC4Context<HookContext>>
class PlantedC4 {
public:
    template <typename... Args>
    explicit PlantedC4(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return context.baseEntity();
    }

    [[nodiscard]] auto getTimeToExplosion() const noexcept
    {
        return context.blowTime().toOptional() - context.curtime();
    }

    [[nodiscard]] auto isTicking() const noexcept
    {
        return context.ticking().toOptional();
    }

    [[nodiscard]] bool isBeingDefused() const noexcept
    {
        return context.defuser().valueOr(cs2::INVALID_EHANDLE_INDEX) != cs2::INVALID_EHANDLE_INDEX;
    }

    [[nodiscard]] auto canBeDefused() const noexcept
    {
        return context.defuseEndTime().toOptional().lessThan(context.blowTime().toOptional());
    }

    [[nodiscard]] auto getTimeToDefuseEnd() const noexcept
    {
        return context.defuseEndTime().toOptional() - context.curtime();
    }

    [[nodiscard]] const char* getBombSiteIconUrl() const noexcept
    {
        constexpr auto INVALID_BOMBSITE_INDEX = -1;
        switch (context.bombSite().valueOr(INVALID_BOMBSITE_INDEX)) {
        case cs2::BOMBSITE_A_INDEX: return cs2::kBombSiteAIconUrl;
        case cs2::BOMBSITE_B_INDEX: return cs2::kBombSiteBIconUrl;
        default: return nullptr;
        }
    }

private:
    Context context;
};
