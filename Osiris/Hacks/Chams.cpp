#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "../Memory.h"

void Chams::render() noexcept
{
    if (config.chams.enabled) {
        static auto vertex = interfaces.materialSystem->findMaterial("dev/glow_rim3d");
        static auto unlit = interfaces.materialSystem->findMaterial("debug/debugdrawflat");
        static bool isInitialized = false;

        if (!isInitialized) {
            vertex->incrementReferenceCount();
            unlit->incrementReferenceCount();
            isInitialized = true;
        }

        auto material = config.chams.shader ? unlit : vertex;

        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);
        interfaces.renderView->setBlend(config.chams.alpha);

        for (int i = 1; i < interfaces.engine->getMaxClients(); ++i) {
            auto entity = interfaces.entityList->getClientEntity(i);

            if (entity && !entity->isDormant() && entity->isAlive()) {
                if (entity->isEnemy()) {
                    if (config.chams.enemies) {
                        interfaces.renderView->setColorModulation(config.chams.enemiesColor);
                        material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                        interfaces.modelRender->forceMaterialOverride(material);
                        entity->drawModel(1, 255);
                    }
                    if (config.chams.visibleEnemies) {
                        interfaces.renderView->setColorModulation(config.chams.visibleEnemiesColor);
                        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                        interfaces.modelRender->forceMaterialOverride(material);
                        entity->drawModel(1, 255);
                    }
                }
                else {
                    if (config.chams.allies) {
                        interfaces.renderView->setColorModulation(config.chams.alliesColor);
                        material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                        interfaces.modelRender->forceMaterialOverride(material);
                        entity->drawModel(1, 255);
                    }
                    if (config.chams.visibleAllies) {
                        interfaces.renderView->setColorModulation(config.chams.visibleAlliesColor);
                        material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                        interfaces.modelRender->forceMaterialOverride(material);
                        entity->drawModel(1, 255);
                    }
                }
            }
        }
        interfaces.modelRender->forceMaterialOverride(nullptr);
    }
}

void Chams::renderDME(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld)
{
    if (config.chams.enabled) {
        if (!strstr(info.model->name, "models/player"))
            return;
        static auto vertex = interfaces.materialSystem->findMaterial("dev/glow_color");
        static auto unlit = interfaces.materialSystem->findMaterial("debug/debugdrawflat");
        static bool isInitialized = false;

        if (!isInitialized) {
            vertex->incrementReferenceCount();
            unlit->incrementReferenceCount();
            isInitialized = true;
        }

        auto material = config.chams.shader ? unlit : vertex;
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);
        material->alphaModulate(config.chams.alpha);

        auto entity = interfaces.entityList->getClientEntity(info.entityIndex);

        if (entity && !entity->isDormant() && entity->isAlive()) {
            if (entity->isEnemy()) {
                if (config.chams.enemies) {
                    interfaces.renderView->setColorModulation(config.chams.enemiesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                    interfaces.modelRender->forceMaterialOverride(material);
                    hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                }
                if (config.chams.visibleEnemies) {
                    interfaces.renderView->setColorModulation(config.chams.visibleEnemiesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                    interfaces.modelRender->forceMaterialOverride(material);
                }
            }
            else {
                if (config.chams.allies) {
                    interfaces.renderView->setColorModulation(config.chams.alliesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                    interfaces.modelRender->forceMaterialOverride(material);
                    hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                }
                if (config.chams.visibleAllies) {
                    interfaces.renderView->setColorModulation(config.chams.visibleAlliesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                    interfaces.modelRender->forceMaterialOverride(material);
                }
            }
        }
        //interfaces.modelRender->forceMaterialOverride(nullptr);
    }
}

void Chams::initialize()
{
    std::ofstream("csgo\\materials\\chamsNormal.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
}
)#";

    std::ofstream("csgo\\materials\\chamsFlat.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
}
)#";
}
