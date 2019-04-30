#pragma once

#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/ModelRender.h"

enum {
    CHAMS_ALLIES_VISIBLE = 0,
    CHAMS_ALLIES_OCCLUDED,
    CHAMS_ENEMIES_VISIBLE,
    CHAMS_ENEMIES_OCCLUDED,
    CHAMS_PLANTING_VISIBLE,
    CHAMS_PLANTING_OCCLUDED,
    CHAMS_DEFUSING_VISIBLE,
    CHAMS_DEFUSING_OCCLUDED,
    CHAMS_LOCALPLAYER,
    CHAMS_WEAPONS,
    CHAMS_HANDS
};

class Chams {
public:
    Chams() noexcept;
    void render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;

    constexpr void renderHands() const noexcept
    {
        if (config.chams[CHAMS_HANDS].enabled)
            applyChams(config.chams[CHAMS_HANDS], false, interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_iHealth"));
    }

    constexpr void renderWeapons() const noexcept
    {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (config.chams[CHAMS_WEAPONS].enabled &&
            !localPlayer->getProperty<bool>("m_bIsScoped"))
            applyChams(config.chams[CHAMS_WEAPONS], false, localPlayer->getProperty<int>("m_iHealth"));
    }

private:
    Material* normal;
    Material* flat;
    Material* palm;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* gold;
    Material* plastic;
    Material* blinking;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return palm;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return gold;
        case 8: return plastic;
        case 9: return blinking;
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
