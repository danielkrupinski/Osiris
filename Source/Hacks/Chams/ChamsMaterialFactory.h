#pragma once

#include <CSGO/KeyValues.h>
#include <CSGO/MaterialSystem.h>
#include "ChamsMaterial.h"
#include "ChamsMaterialKeyValuesFactory.h"

struct ChamsMaterialFactory {
    ChamsMaterialFactory(const ChamsMaterialKeyValuesFactory& keyValuesFactory, csgo::MaterialSystem materialSystem)
        : keyValuesFactory{ keyValuesFactory }, materialSystem{ materialSystem }
    {
    }

    [[nodiscard]] csgo::MaterialPOD* operator()(ChamsMaterial material)
    {
        return materialSystem.createMaterial(toString(material).data(), createMaterialKeyValues(material).getPOD());
    }

private:
    [[nodiscard]] csgo::KeyValues createMaterialKeyValues(ChamsMaterial material)
    {
        switch (material) {
        using enum ChamsMaterial;
        default: assert(false);
        case Normal: return keyValuesFactory.normal();
        case Flat: return keyValuesFactory.flat();
        case Animated: return keyValuesFactory.animated();
        case Platinum: return keyValuesFactory.platinum();
        case Glass: return keyValuesFactory.glass();
        case Crystal: return keyValuesFactory.crystal();
        case Chrome: return keyValuesFactory.chrome();
        case Silver: return keyValuesFactory.silver();
        case Gold: return keyValuesFactory.gold();
        case Plastic: return keyValuesFactory.plastic();
        case Glow: return keyValuesFactory.glow();
        case Pearlescent: return keyValuesFactory.pearlescent();
        case Metallic: return keyValuesFactory.metallic();
        }
    }

    ChamsMaterialKeyValuesFactory keyValuesFactory;
    csgo::MaterialSystem materialSystem;
};
