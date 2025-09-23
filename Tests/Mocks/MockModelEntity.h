#pragma once

#include <type_traits>
#include <gmock/gmock.h>

#include <Mocks/MockGlowProperty.h>
#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockPlayerPawn.h>
#include <Mocks/MockPlantedC4.h>

struct MockModelEntity {
    MOCK_METHOD(MockGlowProperty&, glowProperty, ());
    MOCK_METHOD(MockBaseEntity&, baseEntity, ());
    MOCK_METHOD(MockPlayerPawn&, asPlayerPawn, ());
    MOCK_METHOD(MockPlantedC4&, asPlantedC4, ());

    template <template <typename...> typename EntityType>
    [[nodiscard]] decltype(auto) as() noexcept
    {
        if constexpr (std::is_same_v<EntityType<MockHookContext>, PlayerPawn<MockHookContext>>)
            return asPlayerPawn();
        else if constexpr (std::is_same_v<EntityType<MockHookContext>, PlantedC4<MockHookContext>>)
            return asPlantedC4();
        else
            static_assert(!std::is_same_v<EntityType<MockHookContext>, EntityType<MockHookContext>>, "Unknown type");
    }
};
