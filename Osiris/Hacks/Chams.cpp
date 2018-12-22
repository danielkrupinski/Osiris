#include "Chams.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"

void Chams::render() noexcept
{
    if (config.chams.enabled) {
		// static auto material = interfaces.materialSystem->findMaterial("dev/glow_color");
        static auto vertex = interfaces.materialSystem->findMaterial("debug/debugambientcube");
        static auto unlit = interfaces.materialSystem->findMaterial("debug/debugdrawflat");
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
		// material->setMaterialVarFlag(MaterialVar::IGNOREZ, true);
		// material->setMaterialVarFlag(MaterialVar::WIREFRAME, false);
    }
}
