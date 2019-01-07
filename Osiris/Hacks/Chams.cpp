#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "../Memory.h"

void Chams::renderDME(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld)
{
    if (config.chams.enabled) {
        static bool isInitialized = false;
        static Material* normal;
        static Material* flat;

        if (!isInitialized) {
            Chams::initialize();
            normal = interfaces.materialSystem->findMaterial("chamsNormal");
            flat = interfaces.materialSystem->findMaterial("chamsFlat");
            normal->incrementReferenceCount();
            flat->incrementReferenceCount();
            isInitialized = true;
        }

        auto material = config.chams.flat ? flat : normal;
        auto entity = interfaces.entityList->getClientEntity(info.entityIndex);

        if (entity && !entity->isDormant() && entity->isAlive()) {
            material->alphaModulate(config.chams.alpha);
            material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);

            if (entity->isEnemy()) {
                if (config.chams.enemies) {
                    material->colorModulate(config.chams.enemiesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                    interfaces.modelRender->forceMaterialOverride(material);
                    if (config.chams.visibleEnemies)
                        hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
                }
                if (config.chams.visibleEnemies) {
                    material->colorModulate(config.chams.visibleEnemiesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                    interfaces.modelRender->forceMaterialOverride(material);
                }
            }
            else {
                if (config.chams.allies) {
                    material->colorModulate(config.chams.alliesColor);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                    interfaces.modelRender->forceMaterialOverride(material);
                    if (config.chams.visibleAllies)
                        hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
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

void Chams::initialize()
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
}
