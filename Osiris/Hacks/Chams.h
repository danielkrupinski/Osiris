#pragma once

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/Material.h"
#include "../SDK/ModelRender.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/RenderView.h"

class Chams {
public:
    Chams() noexcept;
    void render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
private:
    void renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;

    constexpr void renderHands() const noexcept
    {
        if (config.chams[HANDS].enabled)
            applyChams(config.chams[HANDS], false, interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_iHealth"));
    }

    constexpr void renderWeapons() const noexcept
    {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (config.chams[WEAPONS].enabled &&
            !localPlayer->getProperty<bool>("m_bIsScoped"))
            applyChams(config.chams[WEAPONS], false, localPlayer->getProperty<int>("m_iHealth"));
    }

    enum ChamsId {
        ALLIES_VISIBLE = 0,
        ALLIES_OCCLUDED,
        ENEMIES_VISIBLE,
        ENEMIES_OCCLUDED,
        PLANTING_VISIBLE,
        PLANTING_OCCLUDED,
        DEFUSING_VISIBLE,
        DEFUSING_OCCLUDED,
        LOCALPLAYER,
        WEAPONS,
        HANDS,
        BACKTRACK
    };

    Material* normal;
    Material* flat;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* gold;
    Material* plastic;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return platinum;
        case 3: return glass;
        case 4: return chrome;
        case 5: return crystal;
        case 6: return gold;
        case 7: return plastic;
        }
    }

    constexpr void applyChams(decltype(config.chams[0])& chams, bool ignorez, int health = 0) const noexcept
    {
        auto material = dispatchMaterial(chams.material);

        if (chams.healthBased && health)
            material->colorModulate(1.0f - health / 100.0f, health / 100.0f, 0.0f);
        else if (chams.rainbow)
            material->colorModulate(sinf(0.6f * memory.globalVars->currenttime) * 0.5f + 0.5f,
                sinf(0.6f * memory.globalVars->currenttime + 2.0f) * 0.5f + 0.5f,
                sinf(0.6f * memory.globalVars->currenttime + 4.0f) * 0.5f + 0.5f);
        else
            material->colorModulate(chams.color);
        material->alphaModulate(chams.alpha * (chams.blinking ? sinf(memory.globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f));

        material->setMaterialVarFlag(MaterialVar::IGNOREZ, ignorez);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, chams.wireframe);
        interfaces.modelRender->forceMaterialOverride(material);
    }
};
