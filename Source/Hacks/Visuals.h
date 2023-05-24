#pragma once

#include "../JsonForward.h"
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>
#include <Platform/Macros/IsPlatform.h>
#include <Config/ResetConfigurator.h>
#include <Utils/ReturnAddress.h>
#include "Visuals/BulletTracers.h"
#include "Visuals/ColorCorrection.h"
#include "Visuals/SkyboxChanger.h"
#include "Visuals/PostProcessingDisabler.h"
#include "Visuals/ScopeOverlayRemover.h"
#include <CSGO/Functions.h>
#include <CSGO/Recv.h>
#include <CSGO/ViewRenderBeams.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include <BytePatterns/ClientPatternFinder.h>
#include <BytePatterns/EnginePatternFinder.h>

namespace csgo { enum class FrameStage; }
class GameEvent;
struct ImDrawList;
class EngineInterfaces;

class Visuals {
public:
    Visuals(const Memory& memory, OtherInterfaces interfaces, ClientInterfaces clientInterfaces, EngineInterfaces engineInterfaces, const ClientPatternFinder& clientPatternFinder, const EnginePatternFinder& enginePatternFinder);

    bool isZoomOn() noexcept;
    bool isDeagleSpinnerOn() noexcept;
    bool shouldRemoveFog() noexcept;
    float viewModelFov() noexcept;
    float fov() noexcept;
    float farZ() noexcept;

    void inverseRagdollGravity() noexcept;
    void colorWorld() noexcept;
    void modifySmoke(csgo::FrameStage stage) noexcept;
    void thirdperson() noexcept;
    void removeVisualRecoil(csgo::FrameStage stage) noexcept;
    void removeBlur(csgo::FrameStage stage) noexcept;
    void updateBrightness() noexcept;
    void removeGrass(csgo::FrameStage stage) noexcept;
    void remove3dSky() noexcept;
    void removeShadows() noexcept;
    void applyZoom(csgo::FrameStage stage) noexcept;
    void applyScreenEffects() noexcept;
    void hitEffect(const csgo::GameEvent* event = nullptr) noexcept;
    void hitMarker(const csgo::GameEvent* event, ImDrawList* drawList = nullptr) noexcept;
    void disablePostProcessing(csgo::FrameStage stage) noexcept;
    void reduceFlashEffect() noexcept;
    bool removeHands(const char* modelName) noexcept;
    bool removeSleeves(const char* modelName) noexcept;
    bool removeWeapons(const char* modelName) noexcept;
    void skybox(csgo::FrameStage stage) noexcept;
    void bulletTracer(const csgo::GameEvent& event) noexcept;
    void drawMolotovHull(ImDrawList* drawList) noexcept;

    void setDrawColorHook(ReturnAddress hookReturnAddress, int& alpha) const noexcept;
    void updateColorCorrectionWeightsHook() const noexcept;
    bool svCheatsGetBoolHook(ReturnAddress hookReturnAddress) const noexcept;
    bool renderSmokeOverlayHook() const noexcept;

    void updateEventListeners(bool forceRemove = false) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Color correction", colorCorrection);
        configurator("Post-processing Disabler", postProcessingDisabler);
        configurator("Scope Overlay Remover", scopeOverlayRemover);
        configurator("Inverse ragdoll gravity", inverseRagdollGravity_).def(false);
        configurator("No fog", noFog).def(false);
        configurator("No 3d sky", no3dSky).def(false);
        configurator("No aim punch", noAimPunch).def(false);
        configurator("No view punch", noViewPunch).def(false);
        configurator("No hands", noHands).def(false);
        configurator("No sleeves", noSleeves).def(false);
        configurator("No weapons", noWeapons).def(false);
        configurator("No smoke", noSmoke).def(false);
        configurator("No blur", noBlur).def(false);
        configurator("No grass", noGrass).def(false);
        configurator("No shadows", noShadows).def(false);
        configurator("Wireframe smoke", wireframeSmoke).def(false);
        configurator("Zoom", zoom).def(false);
        configurator("Skybox Changer", skyboxChanger);
        configurator("Bullet Tracers", bulletTracers);
    }

private:
    const Memory& memory;
    OtherInterfaces interfaces;
    ClientInterfaces clientInterfaces;
    EngineInterfaces engineInterfaces;
    ColorCorrection colorCorrection;
    SkyboxChanger skyboxChanger;
    PostProcessingDisabler postProcessingDisabler;
    ScopeOverlayRemover scopeOverlayRemover;
    ReturnAddress cameraThink;
    BulletTracers bulletTracers;

#if IS_WIN32()
    FunctionInvoker<csgo::RecvProxy> maxFlashAlphaProxy;
#endif

    bool inverseRagdollGravity_;
    bool noFog;
    bool no3dSky;
    bool noAimPunch;
    bool noViewPunch;
    bool noHands;
    bool noSleeves;
    bool noWeapons;
    bool noSmoke;
    bool noBlur;
    bool noGrass;
    bool noShadows;
    bool wireframeSmoke;
    bool zoom;
};
