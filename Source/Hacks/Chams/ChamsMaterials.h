#pragma once

#include <array>
#include <cassert>

#include "ChamsMaterial.h"
#include "ChamsMaterialFactory.h"
#include <CSGO/Material.h>

class ChamsMaterials {
public:
    explicit ChamsMaterials(const ChamsMaterialFactory& factory)
        : materialFactory{ factory }
    {
    }

    static constexpr auto numberOfMaterials = 13;

    [[nodiscard]] csgo::Material getMaterial(ChamsMaterial material)
    {
        const auto materialIndex = static_cast<std::size_t>(material);
        assert(materialIndex < materials.size());
        assert(materialIndex < materialsInitialized.size());

        if (!materialsInitialized[materialIndex]) {
            materials[materialIndex] = materialFactory(material);
            materialsInitialized[materialIndex] = true;
        }
        return csgo::Material::from(retSpoofGadgets->client, materials[materialIndex]);
    }

private:
    ChamsMaterialFactory materialFactory;
    std::array<csgo::MaterialPOD*, numberOfMaterials> materials{};
    std::array<bool, numberOfMaterials> materialsInitialized{};
};
