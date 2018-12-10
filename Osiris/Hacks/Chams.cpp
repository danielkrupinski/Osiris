#include "Chams.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"

void Chams::render() noexcept
{
    if (config.chams.enabled) {
        for (int i = 1; i < interfaces.engineClient->GetMaxClients(); ++i) {
            BaseEntity* entity = interfaces.clientEntityList->getClientEntity(i);

            if (entity && entity->isAlive()) {
                interfaces.renderView->setBlend(config.chams.alpha);
                auto isEnemy = entity->isEnemy();
                if (!isEnemy && config.chams.enemiesOnly)
                    continue;

                    interfaces.renderView->setColorModulation(config.chams.visibleEnemiesColor);
                    interfaces.renderView->setColorModulation(config.chams.visibleAlliesColor);

                auto material = interfaces.materialSystem->findMaterial("dev/glow_color");
                if (material) {
                    material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);

                    if (!config.chams.visibleOnly) {
                        if (isEnemy)
                            interfaces.renderView->setColorModulation(config.chams.occludedEnemiesColor);
                        else
                            interfaces.renderView->setColorModulation(config.chams.occludedAlliesColor);

                        material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                        
                        interfaces.modelRender->forceMaterialOverride(material);
                        entity->drawModel(1, 255);
                    }

                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, false);
                    if (isEnemy)
                        interfaces.renderView->setColorModulation(config.chams.visibleEnemiesColor);
                    else
                        interfaces.renderView->setColorModulation(config.chams.visibleAlliesColor);
                    interfaces.modelRender->forceMaterialOverride(material);
                    entity->drawModel(1, 255);

                    interfaces.modelRender->forceMaterialOverride(nullptr);
                    material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                    material->setMaterialVarFlag(MaterialVar::WIREFRAME, false);
                }
            }
        }
    }
}
