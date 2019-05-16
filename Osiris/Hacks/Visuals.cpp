#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"
#include "../SDK/Entity.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Input.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"
#include "../Interfaces.h"

void Visuals::colorWorld() noexcept
{
    static auto red = interfaces.cvar->findVar("mat_ambient_light_r");
    static auto green = interfaces.cvar->findVar("mat_ambient_light_g");
    static auto blue = interfaces.cvar->findVar("mat_ambient_light_b");
	if (config.visuals.rainbowWorld) {
		red->setValue(sinf(0.6f * memory.globalVars->currenttime) * 0.5f + 0.5f);
		green->setValue(sinf(0.6f * memory.globalVars->currenttime + 2.0f) * 0.5f + 0.5f);
		blue->setValue(sinf(0.6f * memory.globalVars->currenttime + 4.0f) * 0.5f + 0.5f);
	}
	else {
		red->setValue(config.visuals.worldColor[0]);
		green->setValue(config.visuals.worldColor[1]);
		blue->setValue(config.visuals.worldColor[2]);
	}
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
        auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (stage == FrameStage::RENDER_START) {
            aimPunch = localPlayer->getProperty<Vector>("m_aimPunchAngle");
            viewPunch = localPlayer->getProperty<Vector>("m_viewPunchAngle");
            localPlayer->setProperty("m_viewPunchAngle", Vector{ });
            if (!config.misc.recoilCrosshair)
                localPlayer->setProperty("m_aimPunchAngle", Vector{ });
        }
        else if (stage == FrameStage::RENDER_END) {
            localPlayer->setProperty("m_aimPunchAngle", aimPunch);
            localPlayer->setProperty("m_viewPunchAngle", viewPunch);
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
    // detail/detailsprites_survival - dz_blacksite
    // detail/dust_massive_detail_sprites - dz_sirocco
    static auto grass = interfaces.materialSystem->findMaterial("detail/dust_massive_detail_sprites");
    static auto incrementOnce = grass->incrementReferenceCount();
    grass->setMaterialVarFlag(MaterialVar::NO_DRAW, config.visuals.noGrass);
}

void Visuals::remove3dSky() noexcept
{
    static auto sky = interfaces.cvar->findVar("r_3dsky");
    sky->setValue(!config.visuals.no3dSky);
}
