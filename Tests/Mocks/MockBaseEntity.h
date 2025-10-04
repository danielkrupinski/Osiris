#pragma once

#include <type_traits>
#include <CS2/Classes/Color.h>
#include <GameClient/Entities/SmokeGrenadeProjectile.h>
#include <Utils/Optional.h>

struct MockSmokeGrenadeProjectile;

struct MockBaseEntity {
    MOCK_METHOD(void, applyGlowRecursively, (cs2::Color color, int range));
    MOCK_METHOD(void, applyGlowRecursively, (cs2::Color color));
    MOCK_METHOD(void, applySpawnProtectionEffectRecursively, (cs2::Color color));
    MOCK_METHOD(void, removeSpawnProtectionEffectRecursively, ());
    MOCK_METHOD(Optional<bool>, hasOwner, ());
    MOCK_METHOD(MockSmokeGrenadeProjectile&, asSmokeGrenadeProjectile, ());
    
    auto& baseEntity()
    {
        return *this;
    }

    template <template <typename> typename EntityType>
    [[nodiscard]] decltype(auto) as()
    {
        if constexpr (std::is_same_v<EntityType<MockHookContext>, SmokeGrenadeProjectile<MockHookContext>>) {
            return asSmokeGrenadeProjectile();
        }
    }
};
