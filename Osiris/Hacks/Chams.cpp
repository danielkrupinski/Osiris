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
    normal->incrementReferenceCount();
    flat = interfaces.materialSystem->findMaterial("chamsFlat");
    flat->incrementReferenceCount();
    palm = interfaces.materialSystem->findMaterial("models/props/de_dust/hr_dust/foliage/palm_bark_01");
    palm->incrementReferenceCount();
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
        auto activeWeapon = interfaces.entityList->getEntityFromHandle(entity->getProperty<int>("m_hActiveWeapon"));
        if (activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->getProperty<bool>("m_bStartedArming")) {
            if (config.chams[5].enabled) {
                auto material = dispatchMaterial(config.chams[5].material);
                material->colorModulate(config.chams[5].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->alphaModulate(config.chams[5].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[5].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[4].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[4].enabled) {
                auto material = dispatchMaterial(config.chams[4].material);
                material->colorModulate(config.chams[4].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams[4].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[4].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
        else if (entity->getProperty<bool>("m_bIsDefusing")) {
            if (config.chams[7].enabled) {
                auto material = dispatchMaterial(config.chams[7].material);
                material->colorModulate(config.chams[7].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->alphaModulate(config.chams[7].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[7].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[6].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[6].enabled) {
                auto material = dispatchMaterial(config.chams[6].material);
                material->colorModulate(config.chams[6].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams[6].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[6].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
        else if (info.entityIndex == interfaces.engine->getLocalPlayer()) {
            if (config.chams[8].enabled) {
                auto material = dispatchMaterial(config.chams[8].material);
                material->alphaModulate(config.chams[8].alpha);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[8].wireframe);
                material->colorModulate(config.chams[8].color);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
        else if (entity->isEnemy()) {
            if (config.chams[3].enabled) {
                auto material = dispatchMaterial(config.chams[3].material);
                material->colorModulate(config.chams[3].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->alphaModulate(config.chams[3].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[3].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[2].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[2].enabled) {
                auto material = dispatchMaterial(config.chams[2].material);
                material->colorModulate(config.chams[2].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams[2].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[2].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
        else {
            if (config.chams[1].enabled) {
                auto material = dispatchMaterial(config.chams[1].material);
                material->colorModulate(config.chams[1].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->alphaModulate(config.chams[1].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[1].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[0].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[0].enabled) {
                auto material = dispatchMaterial(config.chams[0].material);
                material->colorModulate(config.chams[0].color);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams[0].alpha);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[0].wireframe);
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
    }
}

void Chams::renderWeapons() const noexcept
{
    if (config.chams[9].enabled &&
        !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<bool>("m_bIsScoped")) {
        auto material = dispatchMaterial(config.chams[9].material);
        material->alphaModulate(config.chams[9].alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[9].wireframe);
        material->colorModulate(config.chams[9].color);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}

void Chams::renderHands() const noexcept
{
    if (config.chams[10].enabled) {
        auto material = dispatchMaterial(config.chams[10].material);
        material->alphaModulate(config.chams[10].alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams[10].wireframe);
        material->colorModulate(config.chams[10].color);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}

