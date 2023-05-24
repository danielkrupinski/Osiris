#include <array>
#include <cstring>
#include <string_view>
#include <utility>
#include <vector>

#include <imgui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

#include "../ConfigStructs.h"
#include <fnv.h>
#include "../GameData.h"
#include "../Helpers.h"
#include "../Memory.h"
#include "../imguiCustom.h"
#include "Visuals.h"

#include <CSGO/PODs/ConVar.h>
#include <CSGO/ConVar.h>
#include <CSGO/Cvar.h>
#include <CSGO/Engine.h>
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/Constants/ConVarNames.h>
#include <CSGO/Constants/FrameStage.h>
#include <CSGO/Constants/GameEventNames.h>
#include <CSGO/GameEvent.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/Input.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/Material.h>
#include <CSGO/MaterialSystem.h>
#include <CSGO/ViewRender.h>
#include <CSGO/ViewRenderBeams.h>

#include "../GlobalContext.h"
#include <Interfaces/ClientInterfaces.h>
#include <Config/LoadConfigurator.h>
#include <Config/ResetConfigurator.h>
#include <Config/SaveConfigurator.h>

#include <BytePatterns/ClientPatternFinder.h>

struct VisualsConfig {
    KeyBindToggle zoomKey;
    bool thirdperson{ false };
    KeyBindToggle thirdpersonKey;
    int thirdpersonDistance{ 0 };
    int viewmodelFov{ 0 };
    int fov{ 0 };
    int farZ{ 0 };
    int flashReduction{ 0 };
    float brightness{ 0.0f };
    ColorToggle3 world;
    ColorToggle3 sky;
    bool deagleSpinner{ false };
    int screenEffect{ 0 };
    int hitEffect{ 0 };
    float hitEffectTime{ 0.6f };
    int hitMarker{ 0 };
    float hitMarkerTime{ 0.6f };
    ColorToggle molotovHull{ 1.0f, 0.27f, 0.0f, 0.3f };
} visualsConfig;

static void from_json(const json& j, VisualsConfig& v)
{
    read(j, "Zoom key", v.zoomKey);
    read(j, "Thirdperson", v.thirdperson);
    read(j, "Thirdperson key", v.thirdpersonKey);
    read(j, "Thirdperson distance", v.thirdpersonDistance);
    read(j, "Viewmodel FOV", v.viewmodelFov);
    read(j, "FOV", v.fov);
    read(j, "Far Z", v.farZ);
    read(j, "Flash reduction", v.flashReduction);
    read(j, "Brightness", v.brightness);
    read<value_t::object>(j, "World", v.world);
    read<value_t::object>(j, "Sky", v.sky);
    read(j, "Deagle spinner", v.deagleSpinner);
    read(j, "Screen effect", v.screenEffect);
    read(j, "Hit effect", v.hitEffect);
    read(j, "Hit effect time", v.hitEffectTime);
    read(j, "Hit marker", v.hitMarker);
    read(j, "Hit marker time", v.hitMarkerTime);
    read<value_t::object>(j, "Molotov Hull", v.molotovHull);
}

static void to_json(json& j, VisualsConfig& o)
{
    const VisualsConfig dummy;

    WRITE("Zoom key", zoomKey);
    WRITE("Thirdperson", thirdperson);
    WRITE("Thirdperson key", thirdpersonKey);
    WRITE("Thirdperson distance", thirdpersonDistance);
    WRITE("Viewmodel FOV", viewmodelFov);
    WRITE("FOV", fov);
    WRITE("Far Z", farZ);
    WRITE("Flash reduction", flashReduction);
    WRITE("Brightness", brightness);
    WRITE("World", world);
    WRITE("Sky", sky);
    WRITE("Deagle spinner", deagleSpinner);
    WRITE("Screen effect", screenEffect);
    WRITE("Hit effect", hitEffect);
    WRITE("Hit effect time", hitEffectTime);
    WRITE("Hit marker", hitMarker);
    WRITE("Hit marker time", hitMarkerTime);
    WRITE("Molotov Hull", molotovHull);
}

Visuals::Visuals(const Memory& memory, OtherInterfaces interfaces, ClientInterfaces clientInterfaces, EngineInterfaces engineInterfaces, const ClientPatternFinder& clientPatternFinder, const EnginePatternFinder& enginePatternFinder)
    : memory{ memory },
    interfaces{ interfaces },
    clientInterfaces{ clientInterfaces },
    engineInterfaces{ engineInterfaces },
    skyboxChanger{ interfaces.getCvar(), FunctionInvoker<csgo::R_LoadNamedSkys>{ retSpoofGadgets->engine, enginePatternFinder.loadNamedSkys() } },
    postProcessingDisabler{ clientPatternFinder.disablePostProcessing() },
    scopeOverlayRemover{ clientPatternFinder.scopeDust(), clientPatternFinder.scopeArc(), clientPatternFinder.vignette() },
    bulletTracers{ csgo::ViewRenderBeams::from(retSpoofGadgets->client, clientPatternFinder.viewRenderBeams()), clientInterfaces.getEntityList(), engineInterfaces.getEngine(), memory.globalVars },
    cameraThink{ clientPatternFinder.cameraThink() }
#if IS_WIN32()
    , maxFlashAlphaProxy{ retSpoofGadgets->client, clientPatternFinder.maxFlashAlphaProxy() }
#endif
{
    ResetConfigurator configurator;
    configure(configurator);
}

bool Visuals::isZoomOn() noexcept
{
    return zoom;
}

bool Visuals::isDeagleSpinnerOn() noexcept
{
    return visualsConfig.deagleSpinner;
}

bool Visuals::shouldRemoveFog() noexcept
{
    return noFog;
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

void Visuals::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.getCvar().findVar(csgo::cl_ragdoll_gravity);
    csgo::ConVar::from(retSpoofGadgets->client, ragdollGravity).setValue(inverseRagdollGravity_ ? -600 : 600);
}

void Visuals::colorWorld() noexcept
{
    if (!visualsConfig.world.enabled && !visualsConfig.sky.enabled)
        return;

    for (short h = interfaces.getMaterialSystem().firstMaterial(); h != interfaces.getMaterialSystem().invalidMaterial(); h = interfaces.getMaterialSystem().nextMaterial(h)) {
        const auto mat = csgo::Material::from(retSpoofGadgets->client, interfaces.getMaterialSystem().getMaterial(h));

        if (mat.getPOD() == nullptr || !mat.isPrecached())
            continue;

        const std::string_view textureGroup = mat.getTextureGroupName();

        if (visualsConfig.world.enabled && (textureGroup.starts_with("World") || textureGroup.starts_with("StaticProp"))) {
            if (visualsConfig.world.asColor3().rainbow)
                mat.colorModulate(rainbowColor(memory.globalVars->realtime, visualsConfig.world.asColor3().rainbowSpeed));
            else
                mat.colorModulate(visualsConfig.world.asColor3().color);
        } else if (visualsConfig.sky.enabled && textureGroup.starts_with("SkyBox")) {
            if (visualsConfig.sky.asColor3().rainbow)
                mat.colorModulate(rainbowColor(memory.globalVars->realtime, visualsConfig.sky.asColor3().rainbowSpeed));
            else
                mat.colorModulate(visualsConfig.sky.asColor3().color);
        }
    }
}

void Visuals::modifySmoke(csgo::FrameStage stage) noexcept
{
    if (stage != csgo::FrameStage::RENDER_START && stage != csgo::FrameStage::RENDER_END)
        return;

    static constexpr std::array smokeMaterials{
        "particle/vistasmokev1/vistasmokev1_emods",
        "particle/vistasmokev1/vistasmokev1_emods_impactdust",
        "particle/vistasmokev1/vistasmokev1_fire",
        "particle/vistasmokev1/vistasmokev1_smokegrenade"
    };

    for (const auto mat : smokeMaterials) {
        const auto material = csgo::Material::from(retSpoofGadgets->client, interfaces.getMaterialSystem().findMaterial(mat));
        material.setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == csgo::FrameStage::RENDER_START && noSmoke);
        material.setMaterialVarFlag(MaterialVarFlag::WIREFRAME, stage == csgo::FrameStage::RENDER_START && wireframeSmoke);
    }
}

void Visuals::thirdperson() noexcept
{
    if (!visualsConfig.thirdperson)
        return;

    memory.input->isCameraInThirdPerson = (!visualsConfig.thirdpersonKey.isSet() || visualsConfig.thirdpersonKey.isToggled()) && localPlayer && localPlayer.get().isAlive();
    memory.input->cameraOffset.z = static_cast<float>(visualsConfig.thirdpersonDistance); 
}

void Visuals::removeVisualRecoil(csgo::FrameStage stage) noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive())
        return;

    static csgo::Vector aimPunch;
    static csgo::Vector viewPunch;

    if (stage == csgo::FrameStage::RENDER_START) {
        aimPunch = localPlayer.get().aimPunchAngle();
        viewPunch = localPlayer.get().viewPunchAngle();

        if (noAimPunch)
            localPlayer.get().aimPunchAngle() = csgo::Vector{ };

        if (noViewPunch)
            localPlayer.get().viewPunchAngle() = csgo::Vector{ };

    } else if (stage == csgo::FrameStage::RENDER_END) {
        localPlayer.get().aimPunchAngle() = aimPunch;
        localPlayer.get().viewPunchAngle() = viewPunch;
    }
}

void Visuals::removeBlur(csgo::FrameStage stage) noexcept
{
    if (stage != csgo::FrameStage::RENDER_START && stage != csgo::FrameStage::RENDER_END)
        return;

    static auto blur = csgo::Material::from(retSpoofGadgets->client, interfaces.getMaterialSystem().findMaterial("dev/scope_bluroverlay"));
    blur.setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == csgo::FrameStage::RENDER_START && noBlur);
}

void Visuals::updateBrightness() noexcept
{
    static auto brightness = interfaces.getCvar().findVar(csgo::mat_force_tonemap_scale);
    csgo::ConVar::from(retSpoofGadgets->client, brightness).setValue(visualsConfig.brightness);
}

void Visuals::removeGrass(csgo::FrameStage stage) noexcept
{
    if (stage != csgo::FrameStage::RENDER_START && stage != csgo::FrameStage::RENDER_END)
        return;

    auto getGrassMaterialName = [this]() noexcept -> const char* {
        switch (fnv::hashRuntime(engineInterfaces.getEngine().getLevelName())) {
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
        csgo::Material::from(retSpoofGadgets->client, interfaces.getMaterialSystem().findMaterial(grassMaterialName)).setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == csgo::FrameStage::RENDER_START && noGrass);
}

void Visuals::remove3dSky() noexcept
{
    static auto sky = interfaces.getCvar().findVar(csgo::r_3dsky);
    csgo::ConVar::from(retSpoofGadgets->client, sky).setValue(!no3dSky);
}

void Visuals::removeShadows() noexcept
{
    static auto shadows = interfaces.getCvar().findVar(csgo::cl_csm_enabled);
    csgo::ConVar::from(retSpoofGadgets->client, shadows).setValue(!noShadows);
}

void Visuals::applyZoom(csgo::FrameStage stage) noexcept
{
    if (zoom && localPlayer) {
        if (stage == csgo::FrameStage::RENDER_START && (localPlayer.get().fov() == 90 || localPlayer.get().fovStart() == 90)) {
            if (visualsConfig.zoomKey.isToggled()) {
                localPlayer.get().fov() = 40;
                localPlayer.get().fovStart() = 40;
            }
        }
    }
}

#if IS_WIN32()
#undef xor
#define DRAW_SCREEN_EFFECT(material, memory, engine) \
{ \
    const auto drawFunction = memory.drawScreenEffectMaterial; \
    int w, h; \
    engine.getScreenSize(w, h); \
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
#define DRAW_SCREEN_EFFECT(material, memory, engine) \
{ \
    int w, h; \
    engine.getScreenSize(w, h); \
    reinterpret_cast<void(*)(csgo::MaterialPOD*, int, int, int, int)>(memory.drawScreenEffectMaterial)(material, 0, 0, w, h); \
}
#endif

void Visuals::applyScreenEffects() noexcept
{
    if (!visualsConfig.screenEffect)
        return;

    const auto material = csgo::Material::from(retSpoofGadgets->client, interfaces.getMaterialSystem().findMaterial([] {
        constexpr std::array effects{
            "effects/dronecam",
            "effects/underwater_overlay",
            "effects/healthboost",
            "effects/dangerzone_screen"
        };

        if (visualsConfig.screenEffect <= 2 || static_cast<std::size_t>(visualsConfig.screenEffect - 2) >= effects.size())
            return effects[0];
        return effects[visualsConfig.screenEffect - 2];
    }()));

    if (visualsConfig.screenEffect == 1)
        csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$c0_x")).setValue(0.0f);
    else if (visualsConfig.screenEffect == 2)
        csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$c0_x")).setValue(0.1f);
    else if (visualsConfig.screenEffect >= 4)
        csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$c0_x")).setValue(1.0f);

#if IS_WIN32()
    const auto pod = material.getPOD();
    const auto engine = engineInterfaces.getEngine();
    DRAW_SCREEN_EFFECT(pod, memory, engine)
#endif
}

void Visuals::hitEffect(const csgo::GameEvent* event) noexcept
{
    if (visualsConfig.hitEffect && localPlayer) {
        static float lastHitTime = 0.0f;

        if (event && engineInterfaces.getEngine().getPlayerForUserID(event->getInt("attacker")) == localPlayer.get().getNetworkable().index()) {
            lastHitTime = memory.globalVars->realtime;
            return;
        }

        if (lastHitTime + visualsConfig.hitEffectTime >= memory.globalVars->realtime) {
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

           
            auto material = csgo::Material::from(retSpoofGadgets->client, interfaces.getMaterialSystem().findMaterial(getEffectMaterial()));
            if (visualsConfig.hitEffect == 1)
                csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$c0_x")).setValue(0.0f);
            else if (visualsConfig.hitEffect == 2)
                csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$c0_x")).setValue(0.1f);
            else if (visualsConfig.hitEffect >= 4)
                csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$c0_x")).setValue(1.0f);

#if IS_WIN32()
            const auto pod = material.getPOD();
            const auto engine = engineInterfaces.getEngine();
            DRAW_SCREEN_EFFECT(pod, memory, engine)
#endif
        }
    }
}

void Visuals::hitMarker(const csgo::GameEvent* event, ImDrawList* drawList) noexcept
{
    if (visualsConfig.hitMarker == 0)
        return;

    static float lastHitTime = 0.0f;

    if (event) {
        if (localPlayer && event->getInt("attacker") == localPlayer.get().getUserId(engineInterfaces.getEngine()))
            lastHitTime = memory.globalVars->realtime;
        return;
    }

    if (lastHitTime + visualsConfig.hitMarkerTime < memory.globalVars->realtime)
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

void Visuals::disablePostProcessing(csgo::FrameStage stage) noexcept
{
    postProcessingDisabler.run(stage);
}

void Visuals::reduceFlashEffect() noexcept
{
    if (localPlayer && visualsConfig.flashReduction != 0) {
#if IS_WIN32()
        if (maxFlashAlphaProxy) {
            csgo::recvProxyData data;
            data.recvProp = nullptr;
            data.value._float = 255.0f - visualsConfig.flashReduction * 2.55f;
            maxFlashAlphaProxy(&data, localPlayer.get().getPOD(), nullptr);
        }
#else
        localPlayer.get().flashMaxAlpha() = 255.0f - visualsConfig.flashReduction * 2.55f;
#endif
    }
}

bool Visuals::removeHands(const char* modelName) noexcept
{
    return noHands && std::strstr(modelName, "arms") && !std::strstr(modelName, "sleeve");
}

bool Visuals::removeSleeves(const char* modelName) noexcept
{
    return noSleeves && std::strstr(modelName, "sleeve");
}

bool Visuals::removeWeapons(const char* modelName) noexcept
{
    return noWeapons && std::strstr(modelName, "models/weapons/v_")
        && !std::strstr(modelName, "arms") && !std::strstr(modelName, "tablet")
        && !std::strstr(modelName, "parachute") && !std::strstr(modelName, "fists");
}

void Visuals::skybox(csgo::FrameStage stage) noexcept
{
    skyboxChanger.run(stage);
}

void Visuals::bulletTracer(const csgo::GameEvent& event) noexcept
{
    bulletTracers.onBulletImpact(event);
}

void Visuals::drawMolotovHull(ImDrawList* drawList) noexcept
{
    if (!visualsConfig.molotovHull.enabled)
        return;

    const auto color = Helpers::calculateColor(memory.globalVars->realtime, visualsConfig.molotovHull.asColor4());

    GameData::Lock lock;

    static const auto flameCircumference = [] {
        std::array<csgo::Vector, 72> points;
        for (std::size_t i = 0; i < points.size(); ++i) {
            constexpr auto flameRadius = 60.0f; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/server/cstrike15/Effects/inferno.cpp#L889
            points[i] = csgo::Vector{ flameRadius * std::cos(Helpers::deg2rad(i * (360.0f / points.size()))),
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

void Visuals::setDrawColorHook(ReturnAddress hookReturnAddress, int& alpha) const noexcept
{
    scopeOverlayRemover.setDrawColorHook(hookReturnAddress, alpha);
}

void Visuals::updateColorCorrectionWeightsHook() const noexcept
{
    colorCorrection.run(memory.clientMode);
    scopeOverlayRemover.updateColorCorrectionWeightsHook();
}

bool Visuals::svCheatsGetBoolHook(ReturnAddress hookReturnAddress) const noexcept
{
    return visualsConfig.thirdperson && hookReturnAddress == cameraThink;
}

bool Visuals::renderSmokeOverlayHook() const noexcept
{
    if (noSmoke || wireframeSmoke) {
        memory.viewRender->smokeOverlayAmount = 0.0f;
        return true;
    }
    return false;
}

void Visuals::updateEventListeners(bool forceRemove) noexcept
{
    static DefaultEventListener listener;
    static bool listenerRegistered = false;

    if (bulletTracers.enabled && !listenerRegistered) {
        engineInterfaces.getGameEventManager(memory.getEventDescriptor).addListener(&listener, csgo::bullet_impact);
        listenerRegistered = true;
    } else if ((!bulletTracers.enabled || forceRemove) && listenerRegistered) {
        engineInterfaces.getGameEventManager(memory.getEventDescriptor).removeListener(&listener);
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
    ImGui::Checkbox("Disable post-processing", &postProcessingDisabler.enabled);
    ImGui::Checkbox("Inverse ragdoll gravity", &inverseRagdollGravity_);
    ImGui::Checkbox("No fog", &noFog);
    ImGui::Checkbox("No 3d sky", &no3dSky);
    ImGui::Checkbox("No aim punch", &noAimPunch);
    ImGui::Checkbox("No view punch", &noViewPunch);
    ImGui::Checkbox("No hands", &noHands);
    ImGui::Checkbox("No sleeves", &noSleeves);
    ImGui::Checkbox("No weapons", &noWeapons);
    ImGui::Checkbox("No smoke", &noSmoke);
    ImGui::Checkbox("No blur", &noBlur);
    ImGui::Checkbox("No scope overlay", &scopeOverlayRemover.enabled);
    ImGui::Checkbox("No grass", &noGrass);
    ImGui::Checkbox("No shadows", &noShadows);
    ImGui::Checkbox("Wireframe smoke", &wireframeSmoke);
    ImGui::NextColumn();
    ImGui::Checkbox("Zoom", &zoom);
    ImGui::SameLine();
    ImGui::PushID("Zoom Key");
    ImGui::hotkey("", visualsConfig.zoomKey);
    ImGui::PopID();
    ImGui::Checkbox("Thirdperson", &visualsConfig.thirdperson);
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
    ImGui::PushID(5);
    ImGui::SliderFloat("", &visualsConfig.brightness, 0.0f, 1.0f, "Brightness: %.2f");
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::PushID(6);
    ImGui::Checkbox("", &skyboxChanger.enabled);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Combo("Skybox", &skyboxChanger.skybox, SkyboxChanger::skyboxList.data(), SkyboxChanger::skyboxList.size());
    ImGuiCustom::colorPicker("World color", visualsConfig.world);
    ImGuiCustom::colorPicker("Sky color", visualsConfig.sky);
    ImGui::Checkbox("Deagle spinner", &visualsConfig.deagleSpinner);
    ImGui::Combo("Screen effect", &visualsConfig.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::Combo("Hit effect", &visualsConfig.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::SliderFloat("Hit effect time", &visualsConfig.hitEffectTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Combo("Hit marker", &visualsConfig.hitMarker, "None\0Default (Cross)\0");
    ImGui::SliderFloat("Hit marker time", &visualsConfig.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
    ImGuiCustom::colorPicker("Bullet Tracers", bulletTracers.color.color.data(), &bulletTracers.color.color[3], nullptr, nullptr, &bulletTracers.enabled);
    ImGuiCustom::colorPicker("Molotov Hull", visualsConfig.molotovHull);

    ImGui::Checkbox("Color correction", &colorCorrection.enabled);
    ImGui::SameLine();

    if (bool ccPopup = ImGui::Button("Edit"))
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }
    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

json Visuals::toJson() noexcept
{
    // old way
    json j;
    to_json(j, visualsConfig);
    
    // new way
    SaveConfigurator saveConfigurator;
    configure(saveConfigurator);
    if (const auto saveJson = saveConfigurator.getJson(); saveJson.is_object())
        j.update(saveJson);

    return j;
}

void Visuals::fromJson(const json& j) noexcept
{
    from_json(j, visualsConfig);

    LoadConfigurator configurator{ j };
    configure(configurator);
}

void Visuals::resetConfig() noexcept
{
    visualsConfig = {};
    ResetConfigurator resetConfigurator;
    configure(resetConfigurator);
}
