#pragma once

#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/EntitySystem/CEntityIndex.h>
#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>

template <typename HookContext>
class EntitySystem {
public:
    explicit EntitySystem(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] cs2::CEntityInstance* getEntityFromHandle(cs2::CEntityHandle handle) const noexcept
    {
        const auto entityIndex = handle.index();
        if (!entityIndex.isValid())
            return nullptr;

        const auto entityList = getEntityList();
        if (!entityList)
            return nullptr;

        const auto chunkIndex = entityIndex.value / cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
        if (const auto* const chunk = entityList->chunks[chunkIndex]) {
            const auto indexInChunk = entityIndex.value % cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            if (const auto& entityIndentity = (*chunk)[indexInChunk]; entityIndentity.handle == handle)
                return entityIndentity.entity;
        }
        return nullptr;
    }
    
    template <typename F>
    void iterateEntities(F&& f) const noexcept
    {
        const auto entityList = getEntityList();
        if (!entityList)
            return;

        const auto highestEntityIndex = getHighestEntityIndex();
        for (int i = 0; i <= highestEntityIndex.value; ++i) {
            const auto chunkIndex = i / cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            const auto* const chunk = entityList->chunks[chunkIndex];
            if (!chunk)
                continue;

            const auto indexInChunk = i % cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            if (const auto& entityIndentity = (*chunk)[indexInChunk]; entityIndentity.entity && entityIndentity.handle.index().value == i)
                f(*entityIndentity.entity);
        }
    }

private:
    [[nodiscard]] cs2::CGameEntitySystem* entitySystem() const noexcept
    {
        if (hookContext.gameDependencies().entitySystemDeps.entitySystem)
            return *hookContext.gameDependencies().entitySystemDeps.entitySystem;
        return nullptr;
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return hookContext.gameDependencies().entitySystemDeps.entityListOffset.of(entitySystem()).get();
    }

    [[nodiscard]] auto getHighestEntityIndex() const noexcept
    {
        const auto highestEntityIndex = hookContext.gameDependencies().entitySystemDeps.highestEntityIndexOffset.of(entitySystem()).get();
        if (highestEntityIndex && highestEntityIndex->isValid())
            return *highestEntityIndex;
        return cs2::kMaxValidEntityIndex;
    }

    HookContext& hookContext;
};
