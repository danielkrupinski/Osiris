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
                if (entity->getTeamNumber() != (*memory.localPlayer)->getTeamNumber()) {
                    auto material = interfaces.materialSystem->findMaterial("simple_regular", "Model textures");
                    material->colorModulate(1.0f, 0.08f, 0.58f);
                    interfaces.modelRender->forceMaterialOverride(material);
                }
            }
        }
    }
}
