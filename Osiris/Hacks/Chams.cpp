#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "../Memory.h"

Chams::Chams() noexcept
{
    std::ofstream("csgo\\materials\\chamsNormal.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$model" "1"
}
)#";

    std::ofstream("csgo\\materials\\chamsFlat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$model" "1"
}
)#";
    normal = interfaces.materialSystem->findMaterial("chamsNormal");
    flat = interfaces.materialSystem->findMaterial("chamsFlat");
    normal->incrementReferenceCount();
    flat->incrementReferenceCount();
}

void Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (config.chams.enabled) {
        if (std::string_view{ info.model->name }.find("models/player") != std::string_view::npos)
            renderPlayers(ctx, state, info, customBoneToWorld);
        else if ((std::string_view{ info.model->name }.find("models/weapons/v_") != std::string_view::npos) && (std::string_view{ info.model->name }.find("arms") == std::string_view::npos))
            renderWeapons(ctx, state, info, customBoneToWorld);
    }
}

void Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld)
{
    if (config.chams.enabled) {
        auto entity = interfaces.entityList->getClientEntity(info.entityIndex);

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
}

void Chams::renderWeapons(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (config.chams.weapons) {
        auto entity = interfaces.entityList->getClientEntity(info.entityIndex);
        auto material = config.chams.flat ? flat : normal;

        material->alphaModulate(config.chams.alpha);
        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);
        material->colorModulate(config.chams.weaponsColor);
        interfaces.modelRender->forceMaterialOverride(material);
    }
}
