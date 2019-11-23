#include "../fnv.h"
#include "Visuals.h"

#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Input.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"
#include "../SDK/RenderContext.h"

void Visuals::disablePanoramablur() noexcept
{
	ConVar* blur = { interfaces.cvar->findVar("@panorama_disable_blur") };
	blur->setValue(config.visuals.disablePanoramablur);
}

void Visuals::scheduleUpdate() noexcept
{
	update = true;
}

void Visuals::viewModel() noexcept
{
	static auto view_x{ interfaces.cvar->findVar("viewmodel_offset_x") };
	static auto view_y{ interfaces.cvar->findVar("viewmodel_offset_y") };
	static auto view_z{ interfaces.cvar->findVar("viewmodel_offset_z") };

	view_x->onChangeCallbacks.size = 0;
	view_y->onChangeCallbacks.size = 0;
	view_z->onChangeCallbacks.size = 0;


	if (config.visuals.viewModel) {
		view_x->setValue(config.visuals.viewModel_x);
		view_y->setValue(config.visuals.viewModel_y);
		view_z->setValue(config.visuals.viewModel_z);
	}
}

void Visuals::colorWorld() noexcept
{
	static MaterialSystem* matSys = interfaces.materialSystem;
	if (!update)
		return;
	for (MaterialHandle_t i = matSys->firstMaterial(); i != matSys->invalidMaterial(); i = matSys->nextMaterial(i)) {

		Material* mat = matSys->getMaterial(i);

		if (!mat)
			continue;
		if (strstr(mat->getTextureGroupName(), "World")) {
			mat->alphaModulate(config.visuals.worldAlpha);
			mat->colorModulate(config.visuals.worldColor[0], config.visuals.worldColor[1], config.visuals.worldColor[2]);
		}
	}
	update = false;
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
        material->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, config.visuals.noSmoke);
        material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, config.visuals.wireframeSmoke);
    }

    if (config.visuals.noSmoke || config.visuals.wireframeSmoke)
        *memory.smokeCount = 0;
}

void Visuals::thirdperson(FrameStage stage, Vector& angle, Vector& choked, Vector& unchoked) noexcept
{
    static bool isInThirdperson{ true };
    static float lastTime{ 0.0f };
	auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    if (GetAsyncKeyState(config.visuals.thirdpersonKey) && memory.globalVars->realtime - lastTime > 0.5f) {
        isInThirdperson = !isInThirdperson;
        lastTime = memory.globalVars->realtime;
    }

    if (config.visuals.thirdperson)
		if (stage == FrameStage::RENDER_START) {

			if (memory.input->isCameraInThirdPerson = (!config.visuals.thirdpersonKey || isInThirdperson)
				&& interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive()) {
				if (config.antiAim.third == 0) {
					*reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(localPlayer) + 0x31D8) = choked;
				}
				if (config.antiAim.third == 1) {
					*reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(localPlayer) + 0x31D8) = unchoked;
				}
				if (config.antiAim.third == 2) {
					*reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(localPlayer) + 0x31D8) = angle;
				}
			}
			memory.input->cameraOffset.z = static_cast<float>(config.visuals.thirdpersonDistance);
		}
}

void Visuals::removeVisualRecoil(FrameStage stage) noexcept
{
    static Vector aimPunch;
    static Vector viewPunch;
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (stage == FrameStage::RENDER_START) {
        aimPunch = localPlayer->aimPunchAngle();
        viewPunch = localPlayer->viewPunchAngle();

        if (config.visuals.noAimPunch && !config.misc.recoilCrosshair)
            localPlayer->aimPunchAngle() = Vector{ };

        if (config.visuals.noViewPunch)
            localPlayer->viewPunchAngle() = Vector{ };

    } else if (stage == FrameStage::RENDER_END) {
        localPlayer->aimPunchAngle() = aimPunch;
        localPlayer->viewPunchAngle() = viewPunch;
    }
}

void Visuals::removeBlur() noexcept
{
    static auto blur = interfaces.materialSystem->findMaterial("dev/scope_bluroverlay");
    blur->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, config.visuals.noBlur);
}

void Visuals::updateBrightness() noexcept
{
    static auto brightness = interfaces.cvar->findVar("mat_force_tonemap_scale");
    brightness->setValue(config.visuals.brightness);
}

void Visuals::removeGrass() noexcept
{
	constexpr auto getGrassMaterialName = []() constexpr noexcept -> const char* {
		switch (fnv::hashRuntime(interfaces.engine->getLevelName())) {
		case fnv::hash("dz_blacksite"): return "detail/detailsprites_survival";
		case fnv::hash("dz_sirocco"): return "detail/dust_massive_detail_sprites";
		case fnv::hash("dz_junglety"): return "detail/tropical_grass";
		default: return nullptr;
		}
	};
	if (const auto grassMaterialName = getGrassMaterialName())
		interfaces.materialSystem->findMaterial(grassMaterialName)->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, config.visuals.noGrass);
}

void Visuals::remove3dSky() noexcept
{
    static auto sky = interfaces.cvar->findVar("r_3dsky");
    sky->setValue(!config.visuals.no3dSky);
}

void Visuals::removeShadows() noexcept
{
    static auto shadows = interfaces.cvar->findVar("cl_csm_enabled");
    shadows->setValue(!config.visuals.noShadows);
}

void Visuals::applyZoom(FrameStage stage) noexcept
{
    if (config.visuals.zoom) {
        auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (stage == FrameStage::RENDER_START && localPlayer && (localPlayer->fov() == 90 || localPlayer->fovStart() == 90)) {
            static bool scoped{ false };

            if (GetAsyncKeyState(config.visuals.zoomKey) & 1)
                scoped = !scoped;

            if (scoped) {
                localPlayer->fov() = 40;
                localPlayer->fovStart() = 40;
            }
        }
    }
}

static __declspec(naked) void drawScreenEffectMaterial(Material* material, int x, int y, int width, int height) noexcept
{
    __asm {
        push ebp
        mov ebp, esp
        push height
        push width
        push y
        mov edx, x
        mov ecx, material
        call memory.drawScreenEffectMaterial
        mov esp, ebp
        pop ebp
        ret
    }
}

void Visuals::applyScreenEffects() noexcept
{
    if (config.visuals.screenEffect) {
        constexpr auto getEffectMaterial = [] {
            static constexpr const char* effects[]{
            "effects/dronecam",
            "effects/underwater_overlay",
            "effects/healthboost",
            "effects/dangerzone_screen"
            };

            if (config.visuals.screenEffect <= 2)
                return effects[0];
            return effects[config.visuals.screenEffect - 2];
        };

        auto renderContext = interfaces.materialSystem->getRenderContext();
        renderContext->beginRender();
        int x, y, width, height;
        renderContext->getViewport(x, y, width, height);
        auto material = interfaces.materialSystem->findMaterial(getEffectMaterial());
        if (config.visuals.screenEffect == 1)
            material->findVar("$c0_x")->setValue(0.0f);
        else if (config.visuals.screenEffect == 2)
            material->findVar("$c0_x")->setValue(0.1f);
        else if (config.visuals.screenEffect >= 4)
            material->findVar("$c0_x")->setValue(1.0f);
        drawScreenEffectMaterial(material, 0, 0, width, height);
        renderContext->endRender();
        renderContext->release();
    }
}

void Visuals::hitMarker(GameEvent* event) noexcept
{
    if (config.visuals.hitMarker) {
        static float lastHitTime = 0.0f;

        if (event && interfaces.engine->getPlayerForUserID(event->getInt("attacker")) == interfaces.engine->getLocalPlayer()) {
            lastHitTime = memory.globalVars->realtime;
            return;
        }

        if (lastHitTime + config.visuals.hitMarkerTime >= memory.globalVars->realtime) {
            constexpr auto getEffectMaterial = [] {
                static constexpr const char* effects[]{
                "effects/dronecam",
                "effects/underwater_overlay",
                "effects/healthboost",
                "effects/dangerzone_screen"
                };

                if (config.visuals.hitMarker <= 2)
                    return effects[0];
                return effects[config.visuals.hitMarker - 2];
            };

            auto renderContext = interfaces.materialSystem->getRenderContext();
            renderContext->beginRender();
            int x, y, width, height;
            renderContext->getViewport(x, y, width, height);
            auto material = interfaces.materialSystem->findMaterial(getEffectMaterial());
            if (config.visuals.hitMarker == 1)
                material->findVar("$c0_x")->setValue(0.0f);
            else if (config.visuals.hitMarker == 2)
                material->findVar("$c0_x")->setValue(0.1f);
            else if (config.visuals.hitMarker >= 4)
                material->findVar("$c0_x")->setValue(1.0f);
            drawScreenEffectMaterial(material, 0, 0, width, height);
            renderContext->endRender();
            renderContext->release();
        }
    }
}
