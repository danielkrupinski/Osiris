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

void Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const std::string_view modelName{ info.model->name };
    if (modelName.find("models/player") != std::string_view::npos)
        renderPlayers(ctx, state, info, customBoneToWorld);
    else if (modelName.find("arms") != std::string_view::npos)
        renderHands();
    else if (modelName.find("models/weapons/v_") != std::string_view::npos
        && modelName.find("tablet") == std::string_view::npos
        && modelName.find("parachute") == std::string_view::npos
        && modelName.find("fists") == std::string_view::npos)
        renderWeapons();
}

void Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    auto entity = interfaces.entityList->getEntity(info.entityIndex);

    if (entity && !entity->isDormant() && entity->isAlive()) {
        if (entity->isEnemy()) {
            if (config.chams.chams[3].enabled) {
                auto material = config.chams.chams[3].flat ? flat : normal;
                material->colorModulate(config.chams.chams[3].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->alphaModulate(config.chams.chams[3].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.chams[3].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams.chams[2].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams.chams[2].enabled) {
                auto material = config.chams.chams[2].flat ? flat : normal;
                material->colorModulate(config.chams.chams[2].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams.chams[2].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.chams[2].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
        else {
            if (config.chams.chams[1].enabled) {
                auto material = config.chams.chams[1].flat ? flat : normal;
                material->colorModulate(config.chams.chams[1].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->alphaModulate(config.chams.chams[1].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.chams[1].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams.chams[0].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams.chams[0].enabled) {
                auto material = config.chams.chams[0].flat ? flat : normal;
                material->colorModulate(config.chams.chams[0].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams.chams[0].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.chams[0].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
    }
}

void Chams::renderWeapons() const noexcept
{
    if (config.chams.chams[4].enabled &&
        !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<bool>("m_bIsScoped")) {
        auto material = config.chams.chams[4].flat ? flat : normal;
        material->alphaModulate(config.chams.chams[4].alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.chams[4].wireframe);
        material->colorModulate(config.chams.chams[4].color);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}

void Chams::renderHands() const noexcept
{
    if (config.chams.chams[5].enabled) {
        auto material = config.chams.chams[5].flat ? flat : normal;
        material->alphaModulate(config.chams.chams[5].alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.chams[5].wireframe);
        material->colorModulate(config.chams.chams[5].color);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}
