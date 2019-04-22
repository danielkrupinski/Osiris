#pragma once

#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/ModelRender.h"

class Chams {
public:
    Chams() noexcept;
    void render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;

    constexpr void renderHands() const noexcept
    {
        if (config.chams[10].enabled)
            applyChams(config.chams[10], false, interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_iHealth"));
    }

    constexpr void renderWeapons() const noexcept
    {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (config.chams[9].enabled &&
            !localPlayer->getProperty<bool>("m_bIsScoped"))
            applyChams(config.chams[9], false, localPlayer->getProperty<int>("m_iHealth"));
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

    constexpr void applyChams(decltype(config.chams[0])& chams, bool ignorez, int health = 0) const noexcept
    {
        auto material = dispatchMaterial(chams.material);
        if (chams.healthBased && health) material->colorModulate({ 1.0f - health / 100.0f,  health / 100.0f, 0.0f });
        else material->colorModulate(chams.color);
        material->alphaModulate(chams.alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, ignorez);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, chams.wireframe);
        interfaces.modelRender->forceMaterialOverride(material);
    }
};
