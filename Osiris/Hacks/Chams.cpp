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
                static auto material = interfaces.materialSystem->findMaterial("dev/glow_color");

                material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.chams.wireframe);

                if (isEnemy) {
                    if (config.chams.occludedEnemies) {
                        interfaces.renderView->setColorModulation(config.chams.occludedEnemiesColor);
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
                    if (config.chams.occludedAllies) {
                        interfaces.renderView->setColorModulation(config.chams.occludedAlliesColor);
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
                interfaces.modelRender->forceMaterialOverride(nullptr);
                material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
                material->setMaterialVarFlag(MaterialVar::WIREFRAME, false);

            }
        }
    }
}
