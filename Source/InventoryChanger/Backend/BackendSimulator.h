#pragma once

#include "Loadout.h"

namespace inventory_changer::backend
{

class BackendSimulator {
public:
    [[nodiscard]] const Loadout& getLoadout() const noexcept
    {
        return loadout;
    }

    [[nodiscard]] static BackendSimulator& instance()
    {
        static BackendSimulator backendSimulator;
        return backendSimulator;
    }

private:
    Loadout loadout;
};

}
