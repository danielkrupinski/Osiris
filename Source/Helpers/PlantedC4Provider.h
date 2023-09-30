#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/CUtlVector.h>

#include <MemoryPatterns/ClientPatterns.h>

struct PlantedC4Provider {
    [[nodiscard]] cs2::CPlantedC4* getPlantedC4() const noexcept
    {
        if (plantedC4s && plantedC4s->size > 0)
            return plantedC4s->memory[0];
        return nullptr;
    }

private:
    cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s{ ClientPatterns::plantedC4s() };
};
