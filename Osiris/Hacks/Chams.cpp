#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"

Chams::Chams()
{
    std::ofstream("csgo\\materials\\simple_regular.vmt") <<        
R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
}

Chams::~Chams()
{
    std::remove("csgo\\materials\\simple_regular.vmt");
}

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
