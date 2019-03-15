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
    static constexpr const char* smokeMaterials[]{
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

void Visuals::thirdperson() noexcept
{
    static bool isInThirdperson{ true };
    static float lastTime{ 0.0f };

    if (GetAsyncKeyState(config.visuals.thirdpersonKey) && memory.globalVars->realtime - lastTime > 0.5f) {
        isInThirdperson = !isInThirdperson;
        lastTime = memory.globalVars->realtime;
    }

    if (config.visuals.thirdperson)
        if (memory.input->isCameraInThirdPerson = (!config.visuals.thirdpersonKey || isInThirdperson)
            && interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive())
            memory.input->cameraOffset.z = static_cast<float>(config.visuals.thirdpersonDistance);
}

void Visuals::removeVisualRecoil(FrameStage stage) noexcept
{
    if (config.visuals.noVisualRecoil) {
        static Vector aimPunch;
        static Vector viewPunch;

        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (stage == FrameStage::RENDER_START) {
            aimPunch = localPlayer->getProperty<Vector>("m_aimPunchAngle");
            viewPunch = localPlayer->getProperty<Vector>("m_viewPunchAngle");
            localPlayer->setProperty("m_aimPunchAngle", Vector{ });
            localPlayer->setProperty("m_viewPunchAngle", Vector{ });
        }
        else if (stage == FrameStage::RENDER_END) {
            localPlayer->setProperty("m_aimPunchAngle", aimPunch);
            localPlayer->setProperty("m_viewPunchAngle", aimPunch);
        }
    }
}

void Visuals::removeBlur() noexcept
{
    static auto blur = interfaces.materialSystem->findMaterial("dev/scope_bluroverlay");
    blur->setMaterialVarFlag(MaterialVar::NO_DRAW, config.visuals.noBlur);
}

void Visuals::updateBrightness() noexcept
{
    static auto brightness = interfaces.cvar->findVar("mat_force_tonemap_scale");
    brightness->setValue(config.visuals.brightness);
}

void Visuals::removeGrass() noexcept
{
    static auto grass = interfaces.materialSystem->findMaterial("detail/detailsprites_survival");
    grass->setMaterialVarFlag(MaterialVar::NO_DRAW, config.visuals.noGrass);
}
