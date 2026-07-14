#pragma once

#include <type_traits>

#include <gmock/gmock.h>

#include <GameClient/Entities/C4.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>

struct MockBaseEntity;
struct MockC4;
struct MockHookContext;

struct MockBaseWeapon {
    MockBaseWeapon()
    {
    }

    MOCK_METHOD(std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool), getSceneObjectUpdater, ());
    MOCK_METHOD(void, setSceneObjectUpdater, (std::uint64_t(*updater)(cs2::C_CSWeaponBase*, void*, bool)));
    MOCK_METHOD(MockBaseEntity&, baseEntity, ());
    MOCK_METHOD(bool, isSniperRifle, ());
    MOCK_METHOD(Optional<float>, bulletInaccuracy, ());
    MOCK_METHOD(Optional<cs2::GrenadeKind>, grenadeKind, (), (const));
    MOCK_METHOD(bool, isC4, (), (const));
    MOCK_METHOD(const char*, getName, (), (const));
    MOCK_METHOD(MockC4&, castToC4, ());

    template <template <typename> typename EntityType>
    [[nodiscard]] decltype(auto) cast()
    {
        if constexpr (std::is_same_v<EntityType<MockHookContext>, C4<MockHookContext>>)
            return castToC4();
    }
};
