#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::colorWorld() noexcept
{
    static auto red = interfaces.cvar->findVar("mat_ambient_light_r");
    static auto green = interfaces.cvar->findVar("mat_ambient_light_g");
    static auto blue = interfaces.cvar->findVar("mat_ambient_light_b");

    red->setValue(config.visuals.worldColor[0]);
    green->setValue(config.visuals.worldColor[1]);
    blue->setValue(config.visuals.worldColor[2]);
}

void Visuals::reduceFlashEffect() noexcept
{
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    *reinterpret_cast<float*>(localPlayer + netvars["m_flFlashMaxAlpha"]) = 255.0f - config.visuals.flashReduction * 2.55f;
}

void Visuals::modifySmoke() noexcept
{
    constexpr std::string_view smokeMaterials[]{
        "particle/vistasmokev1/vistasmokev1_emods",
        "particle/vistasmokev1/vistasmokev1_emods_impactdust",
        "particle/vistasmokev1/vistasmokev1_fire",
        "particle/vistasmokev1/vistasmokev1_smokegrenade",
    };

    for (const auto mat : smokeMaterials) {
        auto material = interfaces.materialSystem->findMaterial(mat);
        material->setMaterialVarFlag(MaterialVar::NO_DRAW, config.visuals.noSmoke);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.visuals.wireframeSmoke);
    }

    if (config.visuals.noSmoke || config.visuals.wireframeSmoke)
        *memory.smokeCount = 0;
}

void Visuals::thirdperson()
{
    static bool isInThirdperson{ true };
    static float lastTime{ 0.0f };

    if (GetAsyncKeyState(config.visuals.thirdpersonKey) && memory.globalVars->realtime - lastTime > 0.5f) {
        isInThirdperson = !isInThirdperson;
        lastTime = memory.globalVars->realtime;
    }
    if (memory.input->isCameraInThirdPerson =
        (config.visuals.thirdperson && isInThirdperson
            && interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive()))
        memory.input->cameraOffset.z = static_cast<float>(config.visuals.thirdpersonDistance);
}
