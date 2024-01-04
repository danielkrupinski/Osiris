#pragma once

#include <cassert>

#include <Utils/FieldOffset.h>
#include "Implementation/EntitySystemImpl.h"

class EntitySystem {
public:
    [[nodiscard]] static cs2::CEntityIndex getHighestEntityIndex() noexcept
    {
        if (impl().thisptr && *impl().thisptr)
            return impl().highestEntityIndexOffset.of(*impl().thisptr).valueOr(cs2::CEntityIndex::maxValid());
        return cs2::CEntityIndex::maxValid();
    }

    [[nodiscard]] static cs2::CConcreteEntityList* getEntityList() noexcept
    {
        if (impl().thisptr && *impl().thisptr)
            return impl().entityListOffset.of(*impl().thisptr).get();
        return nullptr;
    }

private:
    [[nodiscard]] static const EntitySystemImpl& impl() noexcept
    {
        return EntitySystemImpl::instance();
    }
};
