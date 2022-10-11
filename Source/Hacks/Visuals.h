#pragma once

#include "../JsonForward.h"

namespace csgo { enum class FrameStage; }
class GameEvent;
struct ImDrawList;

namespace Visuals
{
    bool isThirdpersonOn() noexcept;
    bool isZoomOn() noexcept;
    bool isSmokeWireframe() noexcept;
    bool isDeagleSpinnerOn() noexcept;
    bool shouldRemoveFog() noexcept;
    bool shouldRemoveScopeOverlay() noexcept;
    bool shouldRemoveSmoke() noexcept;
    float viewModelFov() noexcept;
    float fov() noexcept;
    float farZ() noexcept;

    void performColorCorrection(const Memory& memory) noexcept;
    void inverseRagdollGravity(const Interfaces& interfaces) noexcept;
    void colorWorld(const Interfaces& interfaces, const Memory& memory) noexcept;
    void modifySmoke(const Interfaces& interfaces, csgo::FrameStage stage) noexcept;
    void thirdperson(const Memory& memory) noexcept;
    void removeVisualRecoil(csgo::FrameStage stage) noexcept;
    void removeBlur(const Interfaces& interfaces, csgo::FrameStage stage) noexcept;
    void updateBrightness(const Interfaces& interfaces) noexcept;
    void removeGrass(const Engine& engine, const Interfaces& interfaces, csgo::FrameStage stage) noexcept;
    void remove3dSky(const Interfaces& interfaces) noexcept;
    void removeShadows(const Interfaces& interfaces) noexcept;
    void applyZoom(csgo::FrameStage stage) noexcept;
    void applyScreenEffects(const Engine& engine, const Interfaces& interfaces, const Memory& memory) noexcept;
    void hitEffect(const Engine& engine, const Interfaces& interfaces, const Memory& memory, GameEvent* event = nullptr) noexcept;
    void hitMarker(const Engine& engine, const Interfaces& interfaces, const Memory& memory, GameEvent* event, ImDrawList* drawList = nullptr) noexcept;
    void disablePostProcessing(const Memory& memory, csgo::FrameStage stage) noexcept;
    void reduceFlashEffect() noexcept;
    bool removeHands(const char* modelName) noexcept;
    bool removeSleeves(const char* modelName) noexcept;
    bool removeWeapons(const char* modelName) noexcept;
    void skybox(const Interfaces& interfaces, const Memory& memory, csgo::FrameStage stage) noexcept;
    void bulletTracer(const Engine& engine, const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory, GameEvent& event) noexcept;
    void drawMolotovHull(const Memory& memory, ImDrawList* drawList) noexcept;

    inline constexpr std::array skyboxList{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };

    void updateEventListeners(const EngineInterfaces& engineInterfaces, bool forceRemove = false) noexcept;
    void updateInput() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;
    
    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
