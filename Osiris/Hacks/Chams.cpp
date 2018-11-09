#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"

void Chams::render(const ModelRenderInfo& pInfo)
{
    if (config.chams.enabled) {
        if (strstr(pInfo.pModel->szName, "models/player") != nullptr) {
            auto entity = interfaces.clientEntityList->getClientEntity(pInfo.entity_index);
            if (entity && entity->isAlive()) {
                auto material = interfaces.materialSystem->findMaterial("dev/glow_color", "Model textures");
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                material->alphaModulate(config.chams.alpha);
                if (entity->getTeamNumber() != (*memory.localPlayer)->getTeamNumber()) {
                    material->colorModulate(config.chams.enemiesColor[0], config.chams.enemiesColor[1], config.chams.enemiesColor[2]);
                }
                else {
                    material->colorModulate(config.chams.alliesColor[0], config.chams.alliesColor[1], config.chams.alliesColor[2]);
                }
                interfaces.modelRender->forceMaterialOverride(material);
            }
        }
    }
}
