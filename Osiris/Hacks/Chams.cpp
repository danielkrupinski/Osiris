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
                applyChams(config.chams[5], true);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[4].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[4].enabled)
                applyChams(config.chams[4], false);
        } else if (entity->getProperty<bool>("m_bIsDefusing")) {
            if (config.chams[7].enabled) {
                applyChams(config.chams[7], true);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[6].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[6].enabled)
                applyChams(config.chams[6], false);
        } else if (info.entityIndex == interfaces.engine->getLocalPlayer()) {
            if (config.chams[8].enabled)
                applyChams(config.chams[8], false);
        } else if (entity->isEnemy()) {
            if (config.chams[3].enabled) {
                applyChams(config.chams[3], true);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[2].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[2].enabled)
                applyChams(config.chams[2], false);
        } else {
            if (config.chams[1].enabled) {
                applyChams(config.chams[1], true);
                hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                if (!config.chams[0].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[0].enabled)
                applyChams(config.chams[0], false);
        }
    }
}
