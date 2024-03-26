#pragma once

#include <cassert>
#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CEntityIndex.h>

class EntityListWalker {
public:
    EntityListWalker(const cs2::CConcreteEntityList& entityList, cs2::CEntityIndex highestEntityIndex) noexcept
        : entityList{entityList}
        , highestEntityIndex{highestEntityIndex}
    {
        assert(highestEntityIndex.isValid());
    }

    template <typename F>
    void iterateEntities(F&& f) const noexcept
    {
        for (int i = 0; i <= highestEntityIndex.value; ++i) {
            const auto chunkIndex = i / cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            const auto* const chunk = entityList.chunks[chunkIndex];
            if (!chunk)
                continue;

            const auto indexInChunk = i % cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            if (const auto& entityIndentity = (*chunk)[indexInChunk]; entityIndentity.entity && entityIndentity.handle.index().value == i)
                f(*entityIndentity.entity);
        }
    }

private:
    const cs2::CConcreteEntityList& entityList;
    cs2::CEntityIndex highestEntityIndex;
};
