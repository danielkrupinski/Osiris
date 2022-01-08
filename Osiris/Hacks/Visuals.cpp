#include <algorithm>
#include <array>
#include <cstring>
#include <string_view>
#include <utility>
#include <vector>
#include <iostream>
#include <numbers>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <ranges>

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"

#include "../ConfigStructs.h"
#include "../fnv.h"
#include "../GameData.h"
#include "../Helpers.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../imguiCustom.h"
#include "Visuals.h"
#include "../PostProcessing.h"
#include "../PostProcessing.cpp"

#include "../SDK/ConVar.h"
#include "../SDK/ClientClass.h"
#include "../SDK/Cvar.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Input.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"
#include "../SDK/ViewRenderBeams.h"
#include "../SDK/Utils.h"

struct BulletTracers : ColorToggle {
    BulletTracers() : ColorToggle{ 0.0f, 0.75f, 1.0f, 1.0f } {}
};

struct HitEffect2 {
    bool enabled = false;

    enum Type {
        ChromaticAberration = 0,
        Monochrome = 1
    };

    int type = ChromaticAberration;
};

struct VisualsConfig {
    bool disablePostProcessing{ false };
    bool inverseRagdollGravity{ false };
    int inverseRagdollGravityValue{ -600 };
    bool inverseRagdollGravityCustomize{ false };
    bool ragdollForce{ false };
    int ragdollForceValue{ 800 };
    bool ragdollForceCustomize{ false };
    bool noFog{ false };
    bool no3dSky{ false };
    bool noAimPunch{ false };
    bool noViewPunch{ false };
    bool noHands{ false };
    bool noSleeves{ false };
    bool noWeapons{ false };
    bool noSmoke{ false };
    bool noBlur{ false };
    bool noScopeOverlay{ false };
    bool noGrass{ false };
    bool noShadows{ false };
    bool wireframeSmoke{ false };
    bool rainbowCrosshair{ false };
    float rainbowCrosshairSpeed{ 1.0f };
    bool zoom{ false };
    KeyBindToggle zoomKey;
    bool thirdperson{ false };
    bool deadThirdperson{ false };
    KeyBindToggle thirdpersonKey;
    int thirdpersonDistance{ 0 };
    int viewmodelFov{ 0 };
    int fov{ 0 };
    int farZ{ 0 };
    int flashReduction{ 0 };
    float brightness{ 0.0f };
    int skybox{ 0 };
    std::string customSkybox;
    ColorToggle3 world;
    ColorToggle3 sky;
    bool deagleSpinner{ false };
    int screenEffect{ 0 };
    int hitEffect{ 0 };
    float hitEffectTime{ 0.6f };
    int hitMarker{ 0 };
    float hitMarkerTime{ 0.6f };
    BulletTracers bulletTracers;
    HitEffect2 hitEffect2;
    ColorToggle molotovHull{ 1.0f, 0.27f, 0.0f, 0.3f };
    ColorToggle smokeHull{ 0.0f, 0.81f, 1.0f, 0.60f };
    ColorToggle nadeBlast{ 1.0f, 0.0f, 0.09f, 0.51f };

    struct SmokeTimer {
        bool enabled = false;
        Color4 backgroundColor{ 1.0f, 1.0f, 1.0f, 0.5f };
        Color4 timerColor{ 0.0f, 0.0f, 1.0f, 1.0f };
        float timerThickness{ 1.f };
        Color4 textColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    } smokeTimer;

    struct ColorCorrection {
        bool enabled = false;
        float blue = 0.0f;
        float red = 0.0f;
        float mono = 0.0f;
        float saturation = 0.0f;
        float ghost = 0.0f;
        float green = 0.0f;
        float yellow = 0.0f;
    } colorCorrection;

    struct CustomPostProcessing {
        bool enabled = false;
        float worldExposure = 0.0f;
        float modelAmbient = 0.0f;
        float bloomScale = 0.0f;
    } customPostProcessing;

    struct FogController {
        struct FogSettings {
            ColorToggle3 color;
            int iStartDistance{ -1 };
            int iEndDistance{ -1 };
            float flHdrColorScale{ 1.f };
        } Fog, Sky;
    } FogControl;

    bool fullBright{ false };

    float glowOutlineWidth{ 6.0f };

} visualsConfig;

static void from_json(const json& j, VisualsConfig::ColorCorrection& c)
{
    read(j, "Enabled", c.enabled);
    read(j, "Blue", c.blue);
    read(j, "Red", c.red);
    read(j, "Mono", c.mono);
    read(j, "Saturation", c.saturation);
    read(j, "Ghost", c.ghost);
    read(j, "Green", c.green);
    read(j, "Yellow", c.yellow);
}

static void from_json(const json& j, BulletTracers& o)
{
    from_json(j, static_cast<ColorToggle&>(o));
}

static void from_json(const json& j, VisualsConfig::CustomPostProcessing& c)
{
    read(j, "Enabled", c.enabled);
    read(j, "World exposure", c.worldExposure);
    read(j, "Model ambient", c.modelAmbient);
    read(j, "Bloom scale", c.bloomScale);
}

static void from_json(const json& j, HitEffect2& o)
{
    read(j, "Enabled", o.enabled);
    read_number(j, "Type", o.type);
}

static void from_json(const json& j, VisualsConfig& v)
{
    read(j, "Disable post-processing", v.disablePostProcessing);
    read(j, "Inverse ragdoll gravity", v.inverseRagdollGravity);
    read(j, "inverseRagdollGravityValue", v.inverseRagdollGravityValue);
    read(j, "inverseRagdollGravityCustomize", v.inverseRagdollGravityCustomize);
    read(j, "Ragdoll force", v.ragdollForce);
    read(j, "No fog", v.noFog);
    read(j, "No 3d sky", v.no3dSky);
    read(j, "No aim punch", v.noAimPunch);
    read(j, "No view punch", v.noViewPunch);
    read(j, "No hands", v.noHands);
    read(j, "No sleeves", v.noSleeves);
    read(j, "No weapons", v.noWeapons);
    read(j, "No smoke", v.noSmoke);
    read(j, "No blur", v.noBlur);
    read(j, "No scope overlay", v.noScopeOverlay);
    read(j, "No grass", v.noGrass);
    read(j, "No shadows", v.noShadows);
    read(j, "Wireframe smoke", v.wireframeSmoke);
    read(j, "Rainbow crosshair", v.rainbowCrosshair);
    read(j, "Rainbow crosshair speed", v.rainbowCrosshairSpeed);
    read(j, "Full Brightness", v.fullBright);
    read(j, "Zoom", v.zoom);
    read(j, "Zoom key", v.zoomKey);
    read(j, "Thirdperson", v.thirdperson);
    read(j, "deadThirdperson", v.deadThirdperson);
    read(j, "Thirdperson key", v.thirdpersonKey);
    read(j, "Thirdperson distance", v.thirdpersonDistance);
    read(j, "Viewmodel FOV", v.viewmodelFov);
    read(j, "FOV", v.fov);
    read(j, "Far Z", v.farZ);
    read(j, "Flash reduction", v.flashReduction);
    read(j, "Brightness", v.brightness);
    read(j, "Skybox", v.skybox);
    read<value_t::object>(j, "Custom skybox", v.customSkybox);
    read<value_t::object>(j, "World", v.world);
    read<value_t::object>(j, "Sky", v.sky);
    read(j, "Deagle spinner", v.deagleSpinner);
    read(j, "Screen effect", v.screenEffect);
    read(j, "Hit effect", v.hitEffect);
    read(j, "Hit effect time", v.hitEffectTime);
    read(j, "Hit marker", v.hitMarker);
    read(j, "Hit marker time", v.hitMarkerTime);
    read<value_t::object>(j, "Custom post-processing", v.customPostProcessing);
    read<value_t::object>(j, "Color correction", v.colorCorrection);
    read<value_t::object>(j, "Bullet Tracers", v.bulletTracers);
    read<value_t::object>(j, "Molotov Hull", v.molotovHull);
    read<value_t::object>(j, "Smoke timer", v.smokeTimer);
    read<value_t::object>(j, "Smoke Hull", v.smokeHull);
    read<value_t::object>(j, "Nade Blast", v.nadeBlast);
    read<value_t::object>(j, "Hit Effect2", v.hitEffect2);
    read(j, "Glow thickness", v.glowOutlineWidth);
}

static void from_json(const json& j, VisualsConfig::SmokeTimer& s)
{
    read(j, "Enabled", s.enabled);
    read<value_t::object>(j, "Background color", s.backgroundColor);
    read<value_t::object>(j, "Timer color", s.timerColor);
    read(j, "Timer thickness", s.timerThickness);
    read<value_t::object>(j, "Text color", s.textColor);
}

static void to_json(json& j, const VisualsConfig::ColorCorrection& o, const VisualsConfig::ColorCorrection& dummy)
{
    WRITE("Enabled", enabled);
    WRITE("Blue", blue);
    WRITE("Red", red);
    WRITE("Mono", mono);
    WRITE("Saturation", saturation);
    WRITE("Ghost", ghost);
    WRITE("Green", green);
    WRITE("Yellow", yellow);
}

static void to_json(json& j, const VisualsConfig::SmokeTimer& o, const VisualsConfig::SmokeTimer& dummy)
{
    WRITE("Enabled", enabled);
    WRITE("Background color", backgroundColor);
    WRITE("Timer color", timerColor);
    WRITE("Timer thickness", timerThickness);
    WRITE("Text color", textColor);
}

static void to_json(json& j, const BulletTracers& o, const BulletTracers& dummy = {})
{
    to_json(j, static_cast<const ColorToggle&>(o), dummy);
}

static void to_json(json& j, const VisualsConfig::CustomPostProcessing& o, const VisualsConfig::CustomPostProcessing& dummy)
{
    WRITE("Enabled", enabled);
    WRITE("World exposure", worldExposure);
    WRITE("Model ambient", modelAmbient);
    WRITE("Bloom scale", bloomScale);
}

static void to_json(json& j, const HitEffect2& o, const HitEffect2& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Type", type);
}

static void to_json(json& j, const VisualsConfig& o)
{
    const VisualsConfig dummy;

    WRITE("Disable post-processing", disablePostProcessing);
    WRITE("Inverse ragdoll gravity", inverseRagdollGravity);
    WRITE("inverseRagdollGravityValue", inverseRagdollGravityValue);
    WRITE("inverseRagdollGravityCustomize", inverseRagdollGravityCustomize);
    WRITE("Ragdoll force", ragdollForce);
    WRITE("No fog", noFog);
    WRITE("No 3d sky", no3dSky);
    WRITE("No aim punch", noAimPunch);
    WRITE("No view punch", noViewPunch);
    WRITE("No hands", noHands);
    WRITE("No sleeves", noSleeves);
    WRITE("No weapons", noWeapons);
    WRITE("No smoke", noSmoke);
    WRITE("No blur", noBlur);
    WRITE("No scope overlay", noScopeOverlay);
    WRITE("No grass", noGrass);
    WRITE("No shadows", noShadows);
    WRITE("Wireframe smoke", wireframeSmoke);
    WRITE("Rainbow crosshair", rainbowCrosshair);
    WRITE("Rainbow crosshair speed", rainbowCrosshairSpeed);
    WRITE("Full Brightness", fullBright);
    WRITE("Zoom", zoom);
    WRITE("Zoom key", zoomKey);
    WRITE("Thirdperson", thirdperson);
    WRITE("deadThirdperson", deadThirdperson);
    WRITE("Thirdperson key", thirdpersonKey);
    WRITE("Thirdperson distance", thirdpersonDistance);
    WRITE("Viewmodel FOV", viewmodelFov);
    WRITE("FOV", fov);
    WRITE("Far Z", farZ);
    WRITE("Flash reduction", flashReduction);
    WRITE("Brightness", brightness);
    WRITE("Skybox", skybox);
    WRITE("Custom skybox", customSkybox);
    WRITE("World", world);
    WRITE("Sky", sky);
    WRITE("Deagle spinner", deagleSpinner);
    WRITE("Screen effect", screenEffect);
    WRITE("Hit effect", hitEffect);
    WRITE("Hit effect time", hitEffectTime);
    WRITE("Hit marker", hitMarker);
    WRITE("Hit marker time", hitMarkerTime);
    WRITE("Custom post-processing", customPostProcessing);
    WRITE("Color correction", colorCorrection);
    WRITE("Bullet Tracers", bulletTracers);
    WRITE("Molotov Hull", molotovHull);
    WRITE("Smoke timer", smokeTimer);
    WRITE("Smoke Hull", smokeHull);
    WRITE("Nade Blast", nadeBlast);
    WRITE("Hit Effect2", hitEffect2);
    WRITE("Glow thickness", glowOutlineWidth);
}

inline void colorToStr(std::array<float, 3> color, char* buff) {
    sprintf(buff, "%d %d %d",
        (static_cast<int>(color[0] * 255.f)),
        (static_cast<int>(color[1] * 255.f)),
        (static_cast<int>(color[2] * 255.f))
    );
}

static struct FogOptions {
    ConVar* enable;
    ConVar* color;
    ConVar* maxDensity;
    ConVar* hdr;
    ConVar* start;
    ConVar* end;
};

void Visuals::FogControl() noexcept {

    if (!visualsConfig.FogControl.Fog.color.enabled && !visualsConfig.FogControl.Sky.color.enabled)
        return;

    /* Our ConVars */


    static FogOptions Fog, Sky;
    static bool init = false;

    if (!interfaces->engine->isInGame() || !localPlayer.get() || !localPlayer->isAlive())
    {
        init = false;
        return;
    }

    if (!init) { /* Init ConVars, but only once*/
        Fog.enable = interfaces->cvar->findVar("fog_enable");
        Fog.color = interfaces->cvar->findVar("fog_color");
        Fog.maxDensity = interfaces->cvar->findVar("fog_maxdensity");
        Fog.hdr = interfaces->cvar->findVar("fog_hdrcolorscale");
        Fog.start = interfaces->cvar->findVar("fog_start");
        Fog.end = interfaces->cvar->findVar("fog_end");

        Sky.enable = interfaces->cvar->findVar("fog_enableskybox");
        Sky.color = interfaces->cvar->findVar("fog_colorskybox");
        Sky.maxDensity = interfaces->cvar->findVar("fog_maxdensityskybox");
        Sky.hdr = interfaces->cvar->findVar("fog_hdrcolorscaleskybox");
        Sky.start = interfaces->cvar->findVar("fog_startskybox");
        Sky.end = interfaces->cvar->findVar("fog_endskybox");
        init = true;
        ConVar* fog_override{ interfaces->cvar->findVar("fog_override") };
        fog_override->onChangeCallbacks.size = 0;
        fog_override->setValue(1);
    }
}

bool Visuals::isThirdpersonOn() noexcept
{
    return visualsConfig.thirdperson;
}

bool Visuals::isZoomOn() noexcept
{
    return visualsConfig.zoom;
}

bool Visuals::isSmokeWireframe() noexcept
{
    return visualsConfig.wireframeSmoke;
}

bool Visuals::isDeagleSpinnerOn() noexcept
{
    return visualsConfig.deagleSpinner;
}

bool Visuals::shouldRemoveFog() noexcept
{
    return visualsConfig.noFog;
}

bool Visuals::shouldRemoveScopeOverlay() noexcept
{
    return visualsConfig.noScopeOverlay;
}

bool Visuals::shouldRemoveSmoke() noexcept
{
    return visualsConfig.noSmoke;
}

float Visuals::viewModelFov() noexcept
{
    return static_cast<float>(visualsConfig.viewmodelFov);
}

float Visuals::fov() noexcept
{
    return static_cast<float>(visualsConfig.fov);
}

float Visuals::farZ() noexcept
{
    return static_cast<float>(visualsConfig.farZ);
}

void Visuals::performColorCorrection() noexcept
{
    if (const auto& cfg = visualsConfig.colorCorrection; cfg.enabled) {
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x49C, 0x908)) = cfg.blue;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x4A4, 0x918)) = cfg.red;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x4AC, 0x928)) = cfg.mono;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x4B4, 0x938)) = cfg.saturation;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x4C4, 0x958)) = cfg.ghost;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x4CC, 0x968)) = cfg.green;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + WIN32_LINUX(0x4D4, 0x978)) = cfg.yellow;
    }
}

void Visuals::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces->cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(visualsConfig.inverseRagdollGravity ? visualsConfig.inverseRagdollGravityValue : 600);
}

void Visuals::colorWorld() noexcept
{
    if (!visualsConfig.world.enabled && !visualsConfig.sky.enabled)
        return;

    for (short h = interfaces->materialSystem->firstMaterial(); h != interfaces->materialSystem->invalidMaterial(); h = interfaces->materialSystem->nextMaterial(h)) {
        const auto mat = interfaces->materialSystem->getMaterial(h);

        if (!mat || !mat->isPrecached())
            continue;

        const std::string_view textureGroup = mat->getTextureGroupName();

        if (visualsConfig.world.enabled && (textureGroup.starts_with("World") || textureGroup.starts_with("StaticProp"))) {
            if (visualsConfig.world.asColor3().rainbow)
                mat->colorModulate(rainbowColor(visualsConfig.world.asColor3().rainbowSpeed));
            else
                mat->colorModulate(visualsConfig.world.asColor3().color);
        } else if (visualsConfig.sky.enabled && textureGroup.starts_with("SkyBox")) {
            if (visualsConfig.sky.asColor3().rainbow)
                mat->colorModulate(rainbowColor(visualsConfig.sky.asColor3().rainbowSpeed));
            else
                mat->colorModulate(visualsConfig.sky.asColor3().color);
        }
    }
}

void Visuals::modifySmoke(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    constexpr std::array smokeMaterials{
        "particle/vistasmokev1/vistasmokev1_emods",
        "particle/vistasmokev1/vistasmokev1_emods_impactdust",
        "particle/vistasmokev1/vistasmokev1_fire",
        "particle/vistasmokev1/vistasmokev1_smokegrenade"
    };

    for (const auto mat : smokeMaterials) {
        const auto material = interfaces->materialSystem->findMaterial(mat);
        material->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == FrameStage::RENDER_START && visualsConfig.noSmoke);
        material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, stage == FrameStage::RENDER_START && visualsConfig.wireframeSmoke);
    }
}

void Visuals::thirdperson() noexcept
{
    static bool isInThirdperson{ true };
    static float lastTime{ 0.0f };

    if (GetAsyncKeyState(visualsConfig.thirdpersonKey.isSet()) && memory->globalVars->realtime - lastTime > 0.5f) {
        isInThirdperson = !isInThirdperson;
        lastTime = memory->globalVars->realtime;
    }

    if (!visualsConfig.thirdperson)
        return;

    memory->input->isCameraInThirdPerson = (!visualsConfig.thirdpersonKey.isSet() || visualsConfig.thirdpersonKey.isToggled()) && localPlayer && localPlayer->isAlive();
    memory->input->cameraOffset.z = static_cast<float>(visualsConfig.thirdpersonDistance);
    
    if (!localPlayer->isAlive() && visualsConfig.deadThirdperson)
        localPlayer->observerMode() = (!visualsConfig.thirdpersonKey.isSet() || isInThirdperson) ? ObserverMode::OBS_MODE_CHASE : ObserverMode::OBS_MODE_IN_EYE;
}

void Visuals::removeVisualRecoil(FrameStage stage) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    static Vector aimPunch;
    static Vector viewPunch;

    if (stage == FrameStage::RENDER_START) {
        aimPunch = localPlayer->aimPunchAngle();
        viewPunch = localPlayer->viewPunchAngle();

        if (visualsConfig.noAimPunch)
            localPlayer->aimPunchAngle() = Vector{ };

        if (visualsConfig.noViewPunch)
            localPlayer->viewPunchAngle() = Vector{ };

    } else if (stage == FrameStage::RENDER_END) {
        localPlayer->aimPunchAngle() = aimPunch;
        localPlayer->viewPunchAngle() = viewPunch;
    }
}

void Visuals::removeBlur(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static auto blur = interfaces->materialSystem->findMaterial("dev/scope_bluroverlay");
    blur->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == FrameStage::RENDER_START && visualsConfig.noBlur);
}

void Visuals::updateBrightness() noexcept
{
    static auto brightness = interfaces->cvar->findVar("mat_force_tonemap_scale");
    brightness->setValue(visualsConfig.brightness);
}

void Visuals::fullBright() noexcept
{
    const auto bright = interfaces->cvar->findVar("mat_fullbright");
    bright->setValue(visualsConfig.fullBright);
}

void Visuals::ragdollForce() noexcept
{
    const auto force = interfaces->cvar->findVar("phys_pushscale");
    force->setValue(visualsConfig.ragdollForce ? visualsConfig.ragdollForceValue : 1);
}

void Visuals::changeGlowThickness() noexcept
{
    const auto glowWidth = interfaces->cvar->findVar("glow_outline_width");
    glowWidth->setValue(visualsConfig.glowOutlineWidth);
}

void Visuals::removeGrass(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    constexpr auto getGrassMaterialName = []() noexcept -> const char* {
        switch (fnv::hashRuntime(interfaces->engine->getLevelName())) {
        case fnv::hash("dz_blacksite"): return "detail/detailsprites_survival";
        case fnv::hash("dz_sirocco"): return "detail/dust_massive_detail_sprites";
        case fnv::hash("coop_autumn"): return "detail/autumn_detail_sprites";
        case fnv::hash("dz_frostbite"): return "ski/detail/detailsprites_overgrown_ski";
        // dz_junglety has been removed in 7/23/2020 patch
        // case fnv::hash("dz_junglety"): return "detail/tropical_grass";
        default: return nullptr;
        }
    };

    if (const auto grassMaterialName = getGrassMaterialName())
        interfaces->materialSystem->findMaterial(grassMaterialName)->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == FrameStage::RENDER_START && visualsConfig.noGrass);
}

void Visuals::remove3dSky() noexcept
{
    static auto sky = interfaces->cvar->findVar("r_3dsky");
    sky->setValue(!visualsConfig.no3dSky);
}

void Visuals::removeShadows() noexcept
{
    static auto shadows = interfaces->cvar->findVar("cl_csm_enabled");
    shadows->setValue(!visualsConfig.noShadows);
}

void Visuals::applyZoom(FrameStage stage) noexcept
{
    if (visualsConfig.zoom && localPlayer) {
        if (stage == FrameStage::RENDER_START && (localPlayer->fov() == 90 || localPlayer->fovStart() == 90)) {
            if (visualsConfig.zoomKey.isToggled()) {
                localPlayer->fov() = 40;
                localPlayer->fovStart() = 40;
            }
        }
    }
}

#ifdef _WIN32
#undef xor
#define DRAW_SCREEN_EFFECT(material) \
{ \
    const auto drawFunction = memory->drawScreenEffectMaterial; \
    int w, h; \
    interfaces->engine->getScreenSize(w, h); \
    __asm { \
        __asm push h \
        __asm push w \
        __asm push 0 \
        __asm xor edx, edx \
        __asm mov ecx, material \
        __asm call drawFunction \
        __asm add esp, 12 \
    } \
}

#else
#define DRAW_SCREEN_EFFECT(material) \
{ \
    int w, h; \
    interfaces->engine->getScreenSize(w, h); \
    reinterpret_cast<void(*)(Material*, int, int, int, int)>(memory->drawScreenEffectMaterial)(material, 0, 0, w, h); \
}
#endif

void Visuals::applyScreenEffects() noexcept
{
    if (!visualsConfig.screenEffect)
        return;

    const auto material = interfaces->materialSystem->findMaterial([] {
        constexpr std::array effects{
            "effects/dronecam",
            "effects/underwater_overlay",
            "effects/healthboost",
            "effects/dangerzone_screen"
        };

        if (visualsConfig.screenEffect <= 2 || static_cast<std::size_t>(visualsConfig.screenEffect - 2) >= effects.size())
            return effects[0];
        return effects[visualsConfig.screenEffect - 2];
    }());

    if (visualsConfig.screenEffect == 1)
        material->findVar("$c0_x")->setValue(0.0f);
    else if (visualsConfig.screenEffect == 2)
        material->findVar("$c0_x")->setValue(0.1f);
    else if (visualsConfig.screenEffect >= 4)
        material->findVar("$c0_x")->setValue(1.0f);

    DRAW_SCREEN_EFFECT(material)
}

void Visuals::hitEffect(GameEvent* event) noexcept
{
    if (visualsConfig.hitEffect && localPlayer) {
        static float lastHitTime = 0.0f;

        if (event && interfaces->engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer->index()) {
            lastHitTime = memory->globalVars->realtime;
            return;
        }

        if (lastHitTime + visualsConfig.hitEffectTime >= memory->globalVars->realtime) {
            constexpr auto getEffectMaterial = [] {
                static constexpr const char* effects[]{
                "effects/dronecam",
                "effects/underwater_overlay",
                "effects/healthboost",
                "effects/dangerzone_screen"
                };

                if (visualsConfig.hitEffect <= 2)
                    return effects[0];
                return effects[visualsConfig.hitEffect - 2];
            };

           
            auto material = interfaces->materialSystem->findMaterial(getEffectMaterial());
            if (visualsConfig.hitEffect == 1)
                material->findVar("$c0_x")->setValue(0.0f);
            else if (visualsConfig.hitEffect == 2)
                material->findVar("$c0_x")->setValue(0.1f);
            else if (visualsConfig.hitEffect >= 4)
                material->findVar("$c0_x")->setValue(1.0f);

            DRAW_SCREEN_EFFECT(material)
        }
    }
}

void Visuals::hitMarker(GameEvent* event, ImDrawList* drawList) noexcept
{
    if (visualsConfig.hitMarker == 0)
        return;

    static float lastHitTime = 0.0f;

    if (event) {
        if (localPlayer && event->getInt("attacker") == localPlayer->getUserId())
            lastHitTime = memory->globalVars->realtime;
        return;
    }

    if (lastHitTime + visualsConfig.hitMarkerTime < memory->globalVars->realtime)
        return;

    switch (visualsConfig.hitMarker) {
    case 1:
        const auto& mid = ImGui::GetIO().DisplaySize / 2.0f;
        constexpr auto color = IM_COL32(255, 255, 255, 255);
        drawList->AddLine({ mid.x - 10, mid.y - 10 }, { mid.x - 4, mid.y - 4 }, color);
        drawList->AddLine({ mid.x + 10.5f, mid.y - 10.5f }, { mid.x + 4.5f, mid.y - 4.5f }, color);
        drawList->AddLine({ mid.x + 10.5f, mid.y + 10.5f }, { mid.x + 4.5f, mid.y + 4.5f }, color);
        drawList->AddLine({ mid.x - 10, mid.y + 10 }, { mid.x - 4, mid.y + 4 }, color);
        break;
    }
}

void Visuals::disablePostProcessing(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    *memory->disablePostProcessing = stage == FrameStage::RENDER_START && visualsConfig.disablePostProcessing;
}

void Visuals::reduceFlashEffect() noexcept
{
    if (localPlayer)
        localPlayer->flashMaxAlpha() = 255.0f - visualsConfig.flashReduction * 2.55f;
}

bool Visuals::removeHands(const char* modelName) noexcept
{
    return visualsConfig.noHands && std::strstr(modelName, "arms") && !std::strstr(modelName, "sleeve");
}

bool Visuals::removeSleeves(const char* modelName) noexcept
{
    return visualsConfig.noSleeves && std::strstr(modelName, "sleeve");
}

bool Visuals::removeWeapons(const char* modelName) noexcept
{
    return visualsConfig.noWeapons && std::strstr(modelName, "models/weapons/v_")
        && !std::strstr(modelName, "arms") && !std::strstr(modelName, "tablet")
        && !std::strstr(modelName, "parachute") && !std::strstr(modelName, "fists");
}

void Visuals::skybox(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    if (stage == FrameStage::RENDER_START && visualsConfig.skybox > 0 && static_cast<std::size_t>(visualsConfig.skybox) < skyboxList.size() - 1) {
        memory->loadSky(skyboxList[visualsConfig.skybox]);
    }
    else if (static_cast<std::size_t>(visualsConfig.skybox) == 26 && stage == FrameStage::RENDER_START) {
        memory->loadSky(visualsConfig.customSkybox.c_str());
    }
    else {
        static const auto sv_skyname = interfaces->cvar->findVar("sv_skyname");
        memory->loadSky(sv_skyname->string);
    }
}

void Visuals::rainbowCrosshair() noexcept
{
    const auto red = interfaces->cvar->findVar("cl_crosshaircolor_r");
    const auto green = interfaces->cvar->findVar("cl_crosshaircolor_g");
    const auto blue = interfaces->cvar->findVar("cl_crosshaircolor_b");
    const auto color = interfaces->cvar->findVar("cl_crosshaircolor");

    auto [r, g, b] = rainbowColor(visualsConfig.rainbowCrosshairSpeed);
    r *= 255;
    g *= 255;
    b *= 255;

    static bool enabled = false;
    static float backupR;
    static float backupG;
    static float backupB;
    static int backupColor;

    if (visualsConfig.rainbowCrosshair) {
        red->setValue(r);
        green->setValue(g);
        blue->setValue(b);
        color->setValue(5);
        enabled = true;
    }
    else {
        if (enabled) {
            red->setValue(backupR);
            green->setValue(backupG);
            blue->setValue(backupB);
            color->setValue(backupColor);
        }
        backupR = red->getFloat();
        backupG = green->getFloat();
        backupB = blue->getFloat();
        backupColor = color->getInt();
        enabled = false;
    }
}

void Visuals::bulletTracer(GameEvent& event) noexcept
{
    if (!visualsConfig.bulletTracers.enabled)
        return;

    if (!localPlayer)
        return;

    if (event.getInt("userid") != localPlayer->getUserId())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    BeamInfo beamInfo;

    if (!localPlayer->shouldDraw()) {
        const auto viewModel = interfaces->entityList->getEntityFromHandle(localPlayer->viewModel());
        if (!viewModel)
            return;

        if (!viewModel->getAttachment(activeWeapon->getMuzzleAttachmentIndex1stPerson(viewModel), beamInfo.start))
            return;
    } else {
        const auto worldModel = interfaces->entityList->getEntityFromHandle(activeWeapon->weaponWorldModel());
        if (!worldModel)
            return;

        if (!worldModel->getAttachment(activeWeapon->getMuzzleAttachmentIndex3rdPerson(), beamInfo.start))
            return;
    }

    beamInfo.end.x = event.getFloat("x");
    beamInfo.end.y = event.getFloat("y");
    beamInfo.end.z = event.getFloat("z");

    beamInfo.modelName = "sprites/physbeam.vmt";
    beamInfo.modelIndex = -1;
    beamInfo.haloName = nullptr;
    beamInfo.haloIndex = -1;

    beamInfo.red = 255.0f * visualsConfig.bulletTracers.asColor4().color[0];
    beamInfo.green = 255.0f * visualsConfig.bulletTracers.asColor4().color[1];
    beamInfo.blue = 255.0f * visualsConfig.bulletTracers.asColor4().color[2];
    beamInfo.brightness = 255.0f * visualsConfig.bulletTracers.asColor4().color[3];

    beamInfo.type = 0;
    beamInfo.life = 0.0f;
    beamInfo.amplitude = 0.0f;
    beamInfo.segments = -1;
    beamInfo.renderable = true;
    beamInfo.speed = 0.2f;
    beamInfo.startFrame = 0;
    beamInfo.frameRate = 0.0f;
    beamInfo.width = 2.0f;
    beamInfo.endWidth = 2.0f;
    beamInfo.flags = 0x40;
    beamInfo.fadeLength = 20.0f;

    if (const auto beam = memory->viewRenderBeams->createBeamPoints(beamInfo)) {
        constexpr auto FBEAM_FOREVER = 0x4000;
        beam->flags &= ~FBEAM_FOREVER;
        beam->die = memory->globalVars->currenttime + 2.0f;
    }
}

void Visuals::drawMolotovHull(ImDrawList* drawList) noexcept
{
    if (!visualsConfig.molotovHull.enabled)
        return;

    const auto color = Helpers::calculateColor(visualsConfig.molotovHull.asColor4());

    GameData::Lock lock;

    static const auto flameCircumference = [] {
        std::array<Vector, 72> points;
        for (std::size_t i = 0; i < points.size(); ++i) {
            constexpr auto flameRadius = 60.0f; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/server/cstrike15/Effects/inferno.cpp#L889
            points[i] = Vector{ flameRadius * std::cos(Helpers::deg2rad(i * (360.0f / points.size()))),
                                flameRadius * std::sin(Helpers::deg2rad(i * (360.0f / points.size()))),
                                0.0f };
        }
        return points;
    }();

    for (const auto& molotov : GameData::infernos()) {
        for (const auto& pos : molotov.points) {
            std::array<ImVec2, flameCircumference.size()> screenPoints;
            std::size_t count = 0;

            for (const auto& point : flameCircumference) {
                if (Helpers::worldToScreen(pos + point, screenPoints[count]))
                    ++count;
            }

            if (count < 1)
                continue;

            std::swap(screenPoints[0], *std::min_element(screenPoints.begin(), screenPoints.begin() + count, [](const auto& a, const auto& b) { return a.y < b.y || (a.y == b.y && a.x < b.x); }));

            constexpr auto orientation = [](const ImVec2& a, const ImVec2& b, const ImVec2& c) {
                return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
            };

            std::sort(screenPoints.begin() + 1, screenPoints.begin() + count, [&](const auto& a, const auto& b) { return orientation(screenPoints[0], a, b) > 0.0f; });
            drawList->AddConvexPolyFilled(screenPoints.data(), count, color);
        }
    }
}

#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     do { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / ImSqrt(d2); VX *= inv_len; VY *= inv_len; } } while (0)
#define IM_FIXNORMAL2F(VX,VY)               do { float d2 = VX*VX + VY*VY; if (d2 < 0.5f) d2 = 0.5f; float inv_lensq = 1.0f / d2; VX *= inv_lensq; VY *= inv_lensq; } while (0)

static auto generateAntialiasedDot() noexcept
{
    constexpr auto segments = 12;
    constexpr auto radius = 1.0f;

    // based on ImDrawList::PathArcToFast()
    std::array<ImVec2, segments> circleSegments;

    for (int i = 0; i < segments; ++i) {
        const auto data = ImGui::GetDrawListSharedData();
        const ImVec2& c = data->ArcFastVtx[i % IM_ARRAYSIZE(data->ArcFastVtx)];
        circleSegments[i] = ImVec2{ c.x * radius, c.y * radius };
    }

    // based on ImDrawList::AddConvexPolyFilled()
    const float AA_SIZE = 1.0f; // _FringeScale;
    constexpr int idx_count = (segments - 2) * 3 + segments * 6;
    constexpr int vtx_count = (segments * 2);

    std::array<ImDrawIdx, idx_count> indices;
    std::size_t indexIdx = 0;

    // Add indexes for fill
    for (int i = 2; i < segments; ++i) {
        indices[indexIdx++] = 0;
        indices[indexIdx++] = (i - 1) << 1;
        indices[indexIdx++] = i << 1;
    }

    // Compute normals
    std::array<ImVec2, segments> temp_normals;
    for (int i0 = segments - 1, i1 = 0; i1 < segments; i0 = i1++) {
        const ImVec2& p0 = circleSegments[i0];
        const ImVec2& p1 = circleSegments[i1];
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        IM_NORMALIZE2F_OVER_ZERO(dx, dy);
        temp_normals[i0].x = dy;
        temp_normals[i0].y = -dx;
    }

    std::array<ImVec2, vtx_count> vertices;
    std::size_t vertexIdx = 0;

    for (int i0 = segments - 1, i1 = 0; i1 < segments; i0 = i1++) {
        // Average normals
        const ImVec2& n0 = temp_normals[i0];
        const ImVec2& n1 = temp_normals[i1];
        float dm_x = (n0.x + n1.x) * 0.5f;
        float dm_y = (n0.y + n1.y) * 0.5f;
        IM_FIXNORMAL2F(dm_x, dm_y);
        dm_x *= AA_SIZE * 0.5f;
        dm_y *= AA_SIZE * 0.5f;

        vertices[vertexIdx++] = ImVec2{ circleSegments[i1].x - dm_x, circleSegments[i1].y - dm_y };
        vertices[vertexIdx++] = ImVec2{ circleSegments[i1].x + dm_x, circleSegments[i1].y + dm_y };

        indices[indexIdx++] = (i1 << 1);
        indices[indexIdx++] = (i0 << 1);
        indices[indexIdx++] = (i0 << 1) + 1;
        indices[indexIdx++] = (i0 << 1) + 1;
        indices[indexIdx++] = (i1 << 1) + 1;
        indices[indexIdx++] = (i1 << 1);
    }

    return std::make_pair(vertices, indices);
}

template <std::size_t N>
static auto generateSpherePoints() noexcept
{
    constexpr auto goldenAngle = static_cast<float>(2.399963229728653);

    std::array<Vector, N> points;
    for (std::size_t i = 1; i <= points.size(); ++i) {
        const auto latitude = std::asin(2.0f * i / (points.size() + 1) - 1.0f);
        const auto longitude = goldenAngle * i;

        points[i - 1] = Vector{ std::cos(longitude) * std::cos(latitude), std::sin(longitude) * std::cos(latitude), std::sin(latitude) };
    }
    return points;
};

template <std::size_t VTX_COUNT, std::size_t IDX_COUNT>
static void drawPrecomputedPrimitive(ImDrawList* drawList, const ImVec2& pos, ImU32 color, const std::array<ImVec2, VTX_COUNT>& vertices, const std::array<ImDrawIdx, IDX_COUNT>& indices) noexcept
{
    drawList->PrimReserve(indices.size(), vertices.size());

    const ImU32 colors[2]{ color, color & ~IM_COL32_A_MASK };
    const auto uv = ImGui::GetDrawListSharedData()->TexUvWhitePixel;
    for (std::size_t i = 0; i < vertices.size(); ++i) {
        drawList->_VtxWritePtr[i].pos = vertices[i] + pos;
        drawList->_VtxWritePtr[i].uv = uv;
        drawList->_VtxWritePtr[i].col = colors[i & 1];
    }
    drawList->_VtxWritePtr += vertices.size();

    std::memcpy(drawList->_IdxWritePtr, indices.data(), indices.size() * sizeof(ImDrawIdx));

    const auto baseIdx = drawList->_VtxCurrentIdx;
    for (std::size_t i = 0; i < indices.size(); ++i)
        drawList->_IdxWritePtr[i] += baseIdx;

    drawList->_IdxWritePtr += indices.size();
    drawList->_VtxCurrentIdx += vertices.size();
}

static void drawSmokeHull(ImDrawList* drawList) noexcept
{
    if (!visualsConfig.smokeHull.enabled)
        return;

    const auto color = Helpers::calculateColor(visualsConfig.smokeHull.asColor4());

    static const auto spherePoints = generateSpherePoints<2000>();
    static const auto [vertices, indices] = generateAntialiasedDot();

    constexpr auto animationDuration = 0.35f;

    GameData::Lock lock;
    for (const auto& smoke : GameData::smokes()) {
        for (const auto& point : spherePoints) {
            constexpr auto radius = 140.0f;
            if (ImVec2 screenPos; GameData::worldToScreen(smoke.origin + point * Vector{ radius, radius, radius * 0.7f }, screenPos)) {
                drawPrecomputedPrimitive(drawList, screenPos, color, vertices, indices);
            }
        }
    }
}

static void drawNadeBlast(ImDrawList* drawList) noexcept
{
    if (!visualsConfig.nadeBlast.enabled)
        return;

    const auto color = Helpers::calculateColor(visualsConfig.nadeBlast.asColor4());

    static const auto spherePoints = generateSpherePoints<1000>();
    static const auto [vertices, indices] = generateAntialiasedDot();

    constexpr auto blastDuration = 0.35f;

    GameData::Lock lock;
    for (const auto& projectile : GameData::projectiles()) {
        if (!projectile.exploded || projectile.explosionTime + blastDuration < memory->globalVars->realtime)
            continue;

        for (const auto& point : spherePoints) {
            const auto radius = ImLerp(10.0f, 70.0f, (memory->globalVars->realtime - projectile.explosionTime) / blastDuration);
            if (ImVec2 screenPos; GameData::worldToScreen(projectile.coordinateFrame.origin() + point * radius, screenPos)) {
                drawPrecomputedPrimitive(drawList, screenPos, color, vertices, indices);
            }
        }
    }
}

static void hitEffect2(ImDrawList* drawList, GameEvent* event = nullptr) noexcept
{
    if (!visualsConfig.hitEffect2.enabled)
        return;

        constexpr auto effectDuration = 0.3f;
        static float lastHitTime = 0.0f;

    if (event) {
        if (localPlayer && interfaces->engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer->getUserId())
            lastHitTime = memory->globalVars->realtime;
    } else if (lastHitTime + effectDuration >= memory->globalVars->realtime) {
        if (visualsConfig.hitEffect2.type == HitEffect2::ChromaticAberration)
            PostProcessing::performFullscreenChromaticAberration(drawList, (1.0f - (memory->globalVars->realtime - lastHitTime) / effectDuration) * 0.01f);
        else if (visualsConfig.hitEffect2.type == HitEffect2::Monochrome)
            PostProcessing::performFullscreenMonochrome(drawList, (1.0f - (memory->globalVars->realtime - lastHitTime) / effectDuration));
        
    }
}

void Visuals::hitEffect2(GameEvent& event) noexcept
{
    hitEffect2(nullptr, &event);
}

void Visuals::drawPreESP(ImDrawList* drawList) noexcept
{
    drawMolotovHull(drawList);
    drawSmokeHull(drawList);
    drawNadeBlast(drawList);
}

void Visuals::drawPostESP(ImDrawList* drawList) noexcept
{
    /*drawReloadProgress(drawList);*/
    hitEffect2(drawList);
}

#define SMOKEGRENADE_LIFETIME 17.5f

struct SmokeData2
{
    SmokeData2(float destructionTime, Vector pos) : destructionTime{ destructionTime }, pos{ pos } {}

    float destructionTime;
    Vector pos;
    float anim = 1.f;
};

static std::vector<SmokeData2> smokes;

void Visuals::drawSmokeTimerEvent(GameEvent* event) noexcept
{
    smokes.push_back(SmokeData2(
        memory->globalVars->realtime + SMOKEGRENADE_LIFETIME,
        Vector(event->getFloat("x"), event->getFloat("y"), event->getFloat("z"))
    ));
}

void Visuals::drawSmokeTimer(ImDrawList* drawList) noexcept
{
    if (!visualsConfig.smokeTimer.enabled)
        return;

    if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected())
        return;

    for (size_t i = 0; i < smokes.size(); i++) {
        auto& smoke = smokes[i];

        const auto time = std::clamp(smoke.destructionTime - memory->globalVars->realtime, 0.f, SMOKEGRENADE_LIFETIME);
        std::ostringstream text; text << std::fixed << std::showpoint << std::setprecision(1) << time << " sec.";

        const auto text_size = ImGui::CalcTextSize(text.str().c_str());
        ImVec2 pos;

        if (time >= 0.f && smoke.anim >= 0.f) {
            if (Helpers::worldToScreen(smoke.pos, pos)) {

                const auto radius = 10.f + visualsConfig.smokeTimer.timerThickness;
                const auto fraction = std::clamp(time / SMOKEGRENADE_LIFETIME, 0.0f, 1.0f);

                if (time == 0.f)
                    smoke.anim -= 1.f * ImGui::GetIO().DeltaTime;

                Helpers::setAlphaFactor(smoke.anim);
                drawList->AddCircle(pos, radius, Helpers::calculateColor(visualsConfig.smokeTimer.backgroundColor), 40, 3.0f + visualsConfig.smokeTimer.timerThickness);
                if (fraction == 1.0f) {
                    drawList->AddCircle(pos, radius, Helpers::calculateColor(visualsConfig.smokeTimer.timerColor), 40, 2.0f + visualsConfig.smokeTimer.timerThickness);
                }
                else {
                    constexpr float pi = std::numbers::pi_v<float>;
                    const auto arc270 = (3 * pi) / 2;
                    drawList->PathArcTo(pos, radius - 0.5f, arc270 - (2 * pi * fraction), arc270, 40);
                    drawList->PathStroke(Helpers::calculateColor(visualsConfig.smokeTimer.timerColor), false, 2.0f + visualsConfig.smokeTimer.timerThickness);
                }
                drawList->AddText(ImVec2(pos.x - (text_size.x / 2), pos.y + (visualsConfig.smokeTimer.timerThickness * 2.f) + (text_size.y / 2)), Helpers::calculateColor(visualsConfig.smokeTimer.textColor), text.str().c_str());
                Helpers::setAlphaFactor(1.f);
            }
        }
        else
            smokes.erase(smokes.begin() + i);
    }
}

void Visuals::updateEventListeners(bool forceRemove) noexcept
{
    class ImpactEventListener : public GameEventListener {
    public:
        void fireGameEvent(GameEvent* event) override { bulletTracer(*event); }
    };

    static ImpactEventListener listener;
    static bool listenerRegistered = false;

    if (visualsConfig.bulletTracers.enabled && !listenerRegistered) {
        interfaces->gameEventManager->addListener(&listener, "bullet_impact");
        listenerRegistered = true;
    } else if ((!visualsConfig.bulletTracers.enabled || forceRemove) && listenerRegistered) {
        interfaces->gameEventManager->removeListener(&listener);
        listenerRegistered = false;
    }
}

void Visuals::updateInput() noexcept
{
    visualsConfig.thirdpersonKey.handleToggle();
    visualsConfig.zoomKey.handleToggle();
}

static bool windowOpen = false;

void Visuals::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Visuals")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Visuals");
        ImGui::SetWindowPos("Visuals", { 100.0f, 100.0f });
    }
}

void Visuals::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Visuals")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void Visuals::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 680.0f, 0.0f });
        ImGui::Begin("Visuals", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 280.0f);
    ImGui::Checkbox("Disable post-processing", &visualsConfig.disablePostProcessing);
    if (!visualsConfig.disablePostProcessing) {
        ImGui::Checkbox("Custom post-processing", &visualsConfig.customPostProcessing.enabled);
        ImGui::SameLine();
        bool ppPopup = ImGui::Button("EdIt");

        if (ppPopup)
            ImGui::OpenPopup("##pppopup");

        if (ImGui::BeginPopup("##pppopup"))
        {
            ImGui::SliderFloat("World exposure", &visualsConfig.customPostProcessing.worldExposure, 0.0f, 100.f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Model ambient", &visualsConfig.customPostProcessing.modelAmbient, 0.0f, 100.f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Bloom scale", &visualsConfig.customPostProcessing.bloomScale, 0.0f, 100.f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::EndPopup();
        };
    };
    ImGui::Checkbox("Flip ragdoll gravity", &visualsConfig.inverseRagdollGravity);
    if (visualsConfig.inverseRagdollGravity) {
        ImGui::SameLine();
        ImGui::Checkbox("Customize gravity", &visualsConfig.inverseRagdollGravityCustomize);
    };
    if (visualsConfig.inverseRagdollGravityCustomize && visualsConfig.inverseRagdollGravity) {
        ImGui::InputInt("   ", &visualsConfig.inverseRagdollGravityValue, -999999999, 999999999);
    };

    ImGui::Checkbox("Ragdoll force", &visualsConfig.ragdollForce);
    if (visualsConfig.ragdollForce) {
        ImGui::SameLine();
        ImGui::Checkbox("Customize ragdoll force", &visualsConfig.ragdollForceCustomize);
    };
    if (visualsConfig.ragdollForceCustomize && visualsConfig.ragdollForce) {
        ImGui::InputInt("   ", &visualsConfig.ragdollForceValue, -999999999, 999999999);
    };

    ImGui::Checkbox("No fog", &visualsConfig.noFog);
//    if (!visualsConfig.noFog) {
//        ImGuiCustom::colorPicker("Fog color", visualsConfig.FogControl.Fog.color);
//        ImGui::SliderFloat("HDR", &visualsConfig.FogControl.Fog.flHdrColorScale, 0.0f, 255.0f, "%.0f", ImGuiSliderFlags_Logarithmic);
//
//    }
    ImGui::Checkbox("No 3d sky", &visualsConfig.no3dSky);
    ImGui::Checkbox("No aim punch", &visualsConfig.noAimPunch);
    ImGui::Checkbox("No view punch", &visualsConfig.noViewPunch);
    ImGui::Checkbox("No hands", &visualsConfig.noHands);
    ImGui::Checkbox("No sleeves", &visualsConfig.noSleeves);
    ImGui::Checkbox("No weapons", &visualsConfig.noWeapons);
    ImGui::Checkbox("No smoke", &visualsConfig.noSmoke);
    ImGui::Checkbox("No blur", &visualsConfig.noBlur);
    ImGui::Checkbox("No scope overlay", &visualsConfig.noScopeOverlay);
    ImGui::Checkbox("No grass", &visualsConfig.noGrass);
    ImGui::Checkbox("No shadows", &visualsConfig.noShadows);
    ImGui::Checkbox("Wireframe smoke", &visualsConfig.wireframeSmoke);
    ImGui::Checkbox("Rainbow crosshair", &visualsConfig.rainbowCrosshair);
    ImGui::SameLine();
    ImGui::PushItemWidth(100.0f);
    visualsConfig.rainbowCrosshairSpeed = std::clamp(visualsConfig.rainbowCrosshairSpeed, 0.0f, 25.0f);
    ImGui::InputFloat("Speed", &visualsConfig.rainbowCrosshairSpeed, 0.1f, 0.15f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::Checkbox("Hit Effect2", &visualsConfig.hitEffect2.enabled);
    ImGui::PushID("Hit Effect2");
    ImGui::SameLine();

    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SetNextItemWidth(150.0f);
        ImGui::Combo("", &visualsConfig.hitEffect2.type, "Chromatic Aberration\0Monochrome\0");
        ImGui::EndPopup();
    }

    ImGui::Checkbox("Smoke Timer", &visualsConfig.smokeTimer.enabled);
    ImGui::SameLine();
    if (ImGui::Button("...##smoke_timer"))
        ImGui::OpenPopup("##popup_smokeTimer");

    if (ImGui::BeginPopup("##popup_smokeTimer"))
    {
        ImGuiCustom::colorPicker("Background color", visualsConfig.smokeTimer.backgroundColor);
        ImGuiCustom::colorPicker("Text color", visualsConfig.smokeTimer.textColor);
        ImGuiCustom::colorPicker("Timer color", visualsConfig.smokeTimer.timerColor, nullptr, &visualsConfig.smokeTimer.timerThickness);
        ImGui::EndPopup();
    }
    ImGui::PopID();
    ImGui::Checkbox("Full brightness", &visualsConfig.fullBright);
    ImGui::PopID();
    ImGui::NextColumn();
    ImGui::Checkbox("Zoom", &visualsConfig.zoom);
    ImGui::SameLine();
    ImGui::PushID("Zoom Key");
    ImGui::hotkey("", visualsConfig.zoomKey);
    ImGui::PopID();
    ImGui::Checkbox("Thirdperson", &visualsConfig.thirdperson);
    ImGui::SameLine();
    ImGui::Checkbox("Dead thirdperson", &visualsConfig.deadThirdperson);
    ImGui::SameLine();
    ImGui::PushID("Thirdperson Key");
    ImGui::hotkey("", visualsConfig.thirdpersonKey);
    ImGui::PopID();
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(0);
    ImGui::SliderInt("", &visualsConfig.thirdpersonDistance, 0, 1000, "Thirdperson distance: %d");
    ImGui::PopID();
    ImGui::PushID(1);
    ImGui::SliderInt("", &visualsConfig.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
    ImGui::PopID();
    ImGui::PushID(2);
    ImGui::SliderInt("", &visualsConfig.fov, -60, 60, "FOV: %d");
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SliderInt("", &visualsConfig.farZ, 0, 2000, "Far Z: %d");
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SliderInt("", &visualsConfig.flashReduction, 0, 100, "Flash reduction: %d%%");
    ImGui::PopID();
    if (!visualsConfig.customPostProcessing.enabled) {
        ImGui::PushID(5);
        ImGui::SliderFloat("", &visualsConfig.brightness, 0.0f, 1.0f, "Brightness: %.2f");
        ImGui::PopID();
    }
    ImGui::PushID(6);
    ImGui::SliderFloat("", &visualsConfig.glowOutlineWidth, 0.0f, 100.0f, "Glow thickness: %.2f");
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::Combo("Skybox", &visualsConfig.skybox, Visuals::skyboxList.data(), Visuals::skyboxList.size());
    if (visualsConfig.skybox == 26) {
        ImGui::InputText("Skybox filename", &visualsConfig.customSkybox);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("skybox files must be put in csgo/materials/skybox/");
    }
    ImGuiCustom::colorPicker("World color", visualsConfig.world);
    ImGuiCustom::colorPicker("Sky color", visualsConfig.sky);
    ImGui::Checkbox("Deagle spinner", &visualsConfig.deagleSpinner);
    ImGui::Combo("Screen effect", &visualsConfig.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::Combo("Hit effect", &visualsConfig.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::SliderFloat("Hit effect time", &visualsConfig.hitEffectTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Combo("Hit marker", &visualsConfig.hitMarker, "None\0Default (Cross)\0");
    ImGui::SliderFloat("Hit marker time", &visualsConfig.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
    ImGuiCustom::colorPicker("Bullet Tracers", visualsConfig.bulletTracers.asColor4().color.data(), &visualsConfig.bulletTracers.asColor4().color[3], nullptr, nullptr, &visualsConfig.bulletTracers.enabled);
    ImGuiCustom::colorPicker("Molotov Hull", visualsConfig.molotovHull);
    ImGuiCustom::colorPicker("Smoke Hull", visualsConfig.smokeHull);
    ImGuiCustom::colorPicker("Nade Blast", visualsConfig.nadeBlast);

    ImGui::Checkbox("Color correction", &visualsConfig.colorCorrection.enabled);
    ImGui::SameLine();
    
    if (bool ccPopup = ImGui::Button("Edit"))
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &visualsConfig.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }
    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

json Visuals::toJson() noexcept
{
    json j;
    to_json(j, visualsConfig);
    return j;
}

void Visuals::fromJson(const json& j) noexcept
{
    from_json(j, visualsConfig);
}

void Visuals::resetConfig() noexcept
{
    visualsConfig = {};
}

void Visuals::doBloomEffects() noexcept
{
    if (!localPlayer)
        return;
    
    for (int i = 0; i < 2048; i++)
    {
        Entity* ent = interfaces->entityList->getEntity(i);

        if (!ent)
            continue;

        if (!std::string(ent->getClientClass()->networkName).ends_with("TonemapController"))
            continue;

        bool enabled = visualsConfig.customPostProcessing.enabled;
        ent->useCustomAutoExposureMax() = enabled;
        ent->useCustomAutoExposureMin() = enabled;
        ent->useCustomBloomScale() = enabled;

        if (!enabled)
            return;

        float worldExposure = visualsConfig.customPostProcessing.worldExposure;
        ent->customAutoExposureMin() = worldExposure;
        ent->customAutoExposureMax() = worldExposure;

        float bloomScale = visualsConfig.customPostProcessing.bloomScale;
        ent->customBloomScale() = bloomScale;

        ConVar* modelAmbientMin = interfaces->cvar->findVar("r_modelAmbientMin");
        modelAmbientMin->setValue(visualsConfig.customPostProcessing.modelAmbient);
    }
}