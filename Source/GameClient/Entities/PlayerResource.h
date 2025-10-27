#pragma once

template <typename HookContext>
class PlayerResource {
public:
    PlayerResource(HookContext& hookContext, cs2::C_CSPlayerResource* playerResource) noexcept
        : hookContext{hookContext}
        , playerResource{playerResource}
    {
    }

    [[nodiscard]] auto nearestBombsiteIndex(const cs2::Vector& position) const
    {
        return distToBombsiteA(position).lessThan(distToBombsiteB(position)).transform(toBombsiteIndex());
    }

private:
    [[nodiscard]] auto distToBombsiteA(const cs2::Vector& position) const
    {
        return bombsiteACenter().transform(squareDistTo(position));
    }

    [[nodiscard]] auto distToBombsiteB(const cs2::Vector& position) const
    {
        return bombsiteBCenter().transform(squareDistTo(position));
    }

    [[nodiscard]] auto bombsiteACenter() const
    {
        return hookContext.patternSearchResults().template get<OffsetToBombsiteACenter>().of(playerResource).toOptional();
    }

    [[nodiscard]] auto bombsiteBCenter() const
    {
        return hookContext.patternSearchResults().template get<OffsetToBombsiteBCenter>().of(playerResource).toOptional();
    }

    [[nodiscard]] static auto toBombsiteIndex() noexcept
    {
        return [](bool isBombsiteA) {
            return isBombsiteA ? cs2::BombsiteIndex::BombsiteA : cs2::BombsiteIndex::BombsiteB;
        };
    }

    [[nodiscard]] static auto squareDistTo(const cs2::Vector& position) noexcept
    {
        return [&position](const cs2::Vector& bombsiteCenter) {
            return position.squareDistTo(bombsiteCenter);
        };
    }

    HookContext& hookContext;
    cs2::C_CSPlayerResource* playerResource;
};
