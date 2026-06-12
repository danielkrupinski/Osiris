#pragma once

#include "SkyChangerState.h"

class SkyTextureMapping {
public:
    static constexpr std::string_view getSkyTexture(SkyType skyType) noexcept {
        switch (skyType) {
            case SkyType::Mirage:
                return "materials/skybox/mirage/mirage_sky.vtf";
            case SkyType::Inferno:
                return "materials/skybox/inferno/inferno_sky.vtf";
            case SkyType::Nuke:
                return "materials/skybox/nuke/nuke_sky.vtf";
            case SkyType::Dust2:
                return "materials/skybox/dust2/desert_sky.vtf";
            case SkyType::Vertigo:
                return "materials/skybox/vertigo/vertigo_sky.vtf";
            case SkyType::Ancient:
                return "materials/skybox/ancient/ancient_sky.vtf";
            case SkyType::Anubis:
                return "materials/skybox/anubis/anubis_sky.vtf";
            case SkyType::Office:
                return "materials/skybox/office/office_sky.vtf";
            case SkyType::Italy:
                return "materials/skybox/italy/italy_sky.vtf";
            case SkyType::Default:
            default:
                return "materials/skybox/default/default_sky.vtf";
        }
    }

    static constexpr std::string_view getSkyName(SkyType skyType) noexcept {
        switch (skyType) {
            case SkyType::Mirage:
                return "Mirage";
            case SkyType::Inferno:
                return "Inferno";
            case SkyType::Nuke:
                return "Nuke";
            case SkyType::Dust2:
                return "Dust2";
            case SkyType::Vertigo:
                return "Vertigo";
            case SkyType::Ancient:
                return "Ancient";
            case SkyType::Anubis:
                return "Anubis";
            case SkyType::Office:
                return "Office";
            case SkyType::Italy:
                return "Italy";
            case SkyType::Default:
            default:
                return "Default";
        }
    }
};
