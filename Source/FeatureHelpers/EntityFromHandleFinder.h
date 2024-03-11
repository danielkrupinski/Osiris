#pragma once

#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>

class EntityFromHandleFinder {
public:
    explicit EntityFromHandleFinder(const cs2::CConcreteEntityList& entityList) noexcept
        : entityList{entityList}
    {
    }


    [[nodiscard]] cs2::CEntityInstance* getEntityFromHandle(cs2::CEntityHandle handle) const noexcept
    {
        const auto entityIndex = handle.index();
        if (!entityIndex.isValid())
            return nullptr;

        const auto chunkIndex = entityIndex.value / cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
        if (const auto* const chunk = entityList.chunks[chunkIndex]) {
            const auto indexInChunk = entityIndex.value % cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            if (const auto& entityIndentity = (*chunk)[indexInChunk]; entityIndentity.handle == handle)
                return entityIndentity.entity;
        }
        return nullptr;
    }

private:
    const cs2::CConcreteEntityList& entityList;
};
