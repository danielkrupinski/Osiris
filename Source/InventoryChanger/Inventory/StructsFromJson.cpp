#include <nlohmann/json.hpp>

#include "Structs.h"
#include "StructsFromJson.h"

namespace inventory
{

Glove inventory::gloveFromJson(const json& j)
{
    Glove glove;

    if (j.contains("Wear")) {
        if (const auto& wear = j["Wear"]; wear.is_number_float())
            glove.wear = wear;
    }

    if (j.contains("Seed")) {
        if (const auto& seed = j["Seed"]; seed.is_number_integer())
            glove.seed = seed;
    }

    return glove;
}

}
