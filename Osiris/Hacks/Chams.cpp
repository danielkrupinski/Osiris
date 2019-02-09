#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"

Chams::Chams() noexcept
{
    std::ofstream{ "csgo/materials/chamsNormal.vmt" } <<
        "VertexLitGeneric { }";

    std::ofstream{ "csgo/materials/chamsFlat.vmt" } <<
        "UnlitGeneric { }";

    normal = interfaces.materialSystem->findMaterial("chamsNormal");
    flat = interfaces.materialSystem->findMaterial("chamsFlat");
    normal->incrementReferenceCount();
    flat->incrementReferenceCount();
}

void Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (config.chams.enabled) {
        const std::string_view modelName{ info.model->name };
        if (modelName.find("models/player") != std::string_view::npos)
            renderPlayers(ctx, state, info, customBoneToWorld);
        else if (modelName.find("arms") != std::string_view::npos)
            renderHands();
        else if (modelName.find("models/weapons/v_") != std::string_view::npos
            && modelName.find("tablet") == std::string_view::npos
            && modelName.find("parachute") == std::string_view::npos
            && modelName.find("fists") == std::string_view::npos)
            renderWeapons(info.entityIndex);
    }
}

void Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    auto entity = interfaces.entityList->getEntity(info.entityIndex);

    if (entity && !entity->isDormant() && entity->isAlive()) {
        auto material = config.chams.flat ? flat : normal;
        material->alphaModulate(config.chams.alpha);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);

        if (entity->isEnemy()) {
            if (config.chams.occludedEnemies) {
                material->colorModulate(config.chams.occludedEnemiesColor);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams.visibleEnemies)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams.visibleEnemies) {
                material->colorModulate(config.chams.visibleEnemiesColor);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
        else {
            if (config.chams.occludedAllies) {
                material->colorModulate(config.chams.occludedAlliesColor);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams.visibleAllies)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams.visibleAllies) {
                material->colorModulate(config.chams.visibleAlliesColor);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
    }
}

void Chams::renderWeapons(int entityIndex) noexcept
{
    if (config.chams.weapons &&
        !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<bool>("m_bIsScoped")) {
        auto material = config.chams.flat ? flat : normal;

        material->alphaModulate(config.chams.alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);
        material->colorModulate(config.chams.weaponsColor);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}

void Chams::renderHands() noexcept
{
    if (config.chams.hands) {
        auto material = config.chams.flat ? flat : normal;
        material->alphaModulate(config.chams.alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);
        material->colorModulate(config.chams.handsColor);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}
