#pragma once

#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/ModelRender.h"

class Chams {
public:
    Chams() noexcept;
    void render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderWeapons() const noexcept;

    constexpr void renderHands() const noexcept
    {
        if (config.chams[10].enabled)
            applyChams(config.chams[10], false);
    }

private:
    Material* normal;
    Material* flat;
    Material* palm;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return palm;
        }
    }

    constexpr void applyChams(decltype(config.chams[0])& params, bool ignorez) const noexcept
    {
        auto material = dispatchMaterial(params.material);
        material->colorModulate(params.color);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, ignorez);
        material->alphaModulate(params.alpha);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, params.wireframe);
        interfaces.modelRender->forceMaterialOverride(material);
    }
};
