#pragma once

#include <cstdint>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <CS2/Classes/CCSWeaponBaseVData.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <MemoryPatterns/PatternTypes/WeaponVDataPatternTypes.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>

template <typename HookContext>
class BaseWeapon {
public:
    BaseWeapon(HookContext& hookContext, cs2::C_CSWeaponBase* baseWeapon) noexcept
        : hookContext{hookContext}
        , baseWeapon{baseWeapon}
    {
    }

    using RawType = cs2::C_CSWeaponBase;

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(baseWeapon);
    }

    template <template <typename...> typename EntityType>
    [[nodiscard]] bool is() const noexcept
    {
        return baseEntity().template is<EntityType>();
    }

    template <template <typename...> typename EntityType>
    [[nodiscard]] decltype(auto) cast() const noexcept
    {
        return baseEntity().template cast<EntityType>();
    }

    [[nodiscard]] bool isSniperRifle() const noexcept
    {
        switch (baseEntity().classify().typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_WeaponSSG08>():
        case EntityTypeInfo::indexOf<cs2::C_WeaponAWP>():
        case EntityTypeInfo::indexOf<cs2::C_WeaponG3SG1>():
        case EntityTypeInfo::indexOf<cs2::C_WeaponSCAR20>(): return true;
        default: return false;
        }
    }

    [[nodiscard]] auto bulletInaccuracy() const noexcept
    {
        return inaccuracy() + spread();
    }

    [[nodiscard]] auto getName() const noexcept
    {
        const auto vData = static_cast<cs2::CCSWeaponBaseVData*>(baseEntity().vData().valueOr(nullptr));
        return hookContext.patternSearchResults().template get<OffsetToWeaponName>().of(vData).valueOr(nullptr);
    }

    [[nodiscard]] Optional<cs2::GrenadeKind> grenadeKind() const noexcept
    {
        switch (baseEntity().classify().typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_Flashbang>(): return cs2::GrenadeKind::Flashbang;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenade>(): return cs2::GrenadeKind::HEGrenade;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(): return cs2::GrenadeKind::SmokeGrenade;
        case EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>(): return cs2::GrenadeKind::Molotov;
        case EntityTypeInfo::indexOf<cs2::C_DecoyGrenade>(): return cs2::GrenadeKind::Decoy;
        case EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(): return cs2::GrenadeKind::Incendiary;
        default: break;
        }
        const auto name = getName();
        if (stringsEqual(name, "weapon_flashbang")) return cs2::GrenadeKind::Flashbang;
        if (stringsEqual(name, "weapon_hegrenade")) return cs2::GrenadeKind::HEGrenade;
        if (stringsEqual(name, "weapon_smokegrenade")) return cs2::GrenadeKind::SmokeGrenade;
        if (stringsEqual(name, "weapon_molotov")) return cs2::GrenadeKind::Molotov;
        if (stringsEqual(name, "weapon_decoy")) return cs2::GrenadeKind::Decoy;
        if (stringsEqual(name, "weapon_incgrenade")) return cs2::GrenadeKind::Incendiary;
        return {};
    }

    [[nodiscard]] bool isC4() const noexcept
    {
        return baseEntity().classify().template is<cs2::C_C4>() || stringsEqual(getName(), "weapon_c4");
    }

    [[nodiscard]] auto clipAmmo() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToClipAmmo>().of(baseWeapon).toOptional();
    }

    [[nodiscard]] auto getSceneObjectUpdater() const noexcept
    {
        return reinterpret_cast<std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool)>(sceneObjectUpdaterHandle() ? sceneObjectUpdaterHandle()->updaterFunction : nullptr);
    }

    void setSceneObjectUpdater(auto x) const noexcept
    {
        if (sceneObjectUpdaterHandle())
            sceneObjectUpdaterHandle()->updaterFunction = reinterpret_cast<std::uint64_t(*)(void*, void*, bool)>(x);
    }

private:
    [[nodiscard]] static bool stringsEqual(const char* lhs, const char* rhs) noexcept
    {
        if (!lhs || !rhs)
            return false;
        while (*lhs == *rhs) {
            if (*lhs == '\0')
                return true;
            ++lhs;
            ++rhs;
        }
        return false;
    }

    [[nodiscard]] Optional<float> inaccuracy() const noexcept
    {
        const auto getInaccuracyFn = hookContext.patternSearchResults().template get<PointerToGetInaccuracyFunction>();
        if (baseWeapon && getInaccuracyFn)
            return getInaccuracyFn(baseWeapon, nullptr, nullptr);
        return {};
    }

    [[nodiscard]] Optional<float> spread() const noexcept
    {
        const auto getSpreadFn = hookContext.patternSearchResults().template get<PointerToGetSpreadFunction>();
        if (baseWeapon && getSpreadFn)
            return getSpreadFn(baseWeapon);
        return {};
    }

    [[nodiscard]] auto sceneObjectUpdaterHandle() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToWeaponSceneObjectUpdaterHandle>().of(baseWeapon).valueOr(nullptr);
    }

    HookContext& hookContext;
    cs2::C_CSWeaponBase* baseWeapon;
};
