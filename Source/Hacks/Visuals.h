#pragma once

#include "../JsonForward.h"
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>
#include <Platform/IsPlatform.h>
#include <Config/ResetConfigurator.h>

namespace csgo { enum class FrameStage; }
class GameEvent;
struct ImDrawList;
class EngineInterfaces;

class Visuals {
public:
    Visuals(const Memory& memory, OtherInterfaces interfaces, ClientInterfaces clientInterfaces, EngineInterfaces engineInterfaces, const helpers::PatternFinder& clientPatternFinder)
        : memory{ memory }, interfaces{ interfaces }, clientInterfaces{ clientInterfaces }, engineInterfaces{ engineInterfaces }
    {
#if IS_WIN32()
        disablePostProcessingPtr = reinterpret_cast<bool*>(clientPatternFinder("\x83\xEC\x4C\x80\x3D").add(5).deref().get());
#elif IS_LINUX()
        disablePostProcessingPtr = reinterpret_cast<bool*>(clientPatternFinder("\x80\x3D?????\x89\xB5").add(2).relativeToAbsolute().get());
#endif
    }

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

    void performColorCorrection() noexcept;
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
    void hitEffect(const GameEvent* event = nullptr) noexcept;
    void hitMarker(const GameEvent* event, ImDrawList* drawList = nullptr) noexcept;
    void disablePostProcessing(csgo::FrameStage stage) noexcept;
    void reduceFlashEffect() noexcept;
    bool removeHands(const char* modelName) noexcept;
    bool removeSleeves(const char* modelName) noexcept;
    bool removeWeapons(const char* modelName) noexcept;
    void skybox(csgo::FrameStage stage) noexcept;
    void bulletTracer(const GameEvent& event) noexcept;
    void drawMolotovHull(ImDrawList* drawList) noexcept;

    static constexpr std::array skyboxList{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };

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

private:
    const Memory& memory;
    OtherInterfaces interfaces;
    ClientInterfaces clientInterfaces;
    EngineInterfaces engineInterfaces;
    bool* disablePostProcessingPtr;

    struct ColorCorrection {
        bool enabled;
        float blue;
        float red;
        float mono;
        float saturation;
        float ghost;
        float green;
        float yellow;
        
        ColorCorrection()
        {
            ResetConfigurator configurator;
            configure(configurator);
        }

        void run(ClientMode* clientMode) const noexcept
        {
            if (!enabled)
                return;

            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x49C, 0x908)) = blue;
            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4A4, 0x918)) = red;
            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4AC, 0x928)) = mono;
            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4B4, 0x938)) = saturation;
            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4C4, 0x958)) = ghost;
            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4CC, 0x968)) = green;
            *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4D4, 0x978)) = yellow;
        }

        template <typename Configurator>
        void configure(Configurator& configurator)
        {
            configurator("Enabled", enabled).def(false);
            configurator("Blue", blue).def(0.0f);
            configurator("Red", red).def(0.0f);
            configurator("Mono", mono).def(0.0f);
            configurator("Saturation", saturation).def(0.0f);
            configurator("Ghost", ghost).def(0.0f);
            configurator("Green", green).def(0.0f);
            configurator("Yellow", yellow).def(0.0f);
        }
    } colorCorrection;
};
