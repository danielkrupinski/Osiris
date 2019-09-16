#pragma once

#include <array>
#include <filesystem>

#include "imgui/imgui.h"
#include "nSkinz/config_.hpp"

class Config {
public:
    explicit Config(const char*) noexcept;
    void load(size_t) noexcept;
    void save(size_t) const noexcept;
    void add(const char*) noexcept;
    void remove(size_t) noexcept;
    void rename(size_t, const char*) noexcept;
    void reset() noexcept;

    constexpr auto& getConfigs() noexcept
    {
        return configs;
    }

    struct Aimbot {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        int keyMode{ 0 };
        bool aimlock{ false };
        bool silent{ false };
        bool friendlyFire{ false };
        bool visibleOnly{ true };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool autoScope{ false };
        bool recoilbasedFov{ false };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float recoilControlX{ 0.0f };
        float recoilControlY{ 0.0f };
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
        int minDamage{ 1 };
        bool killshot{ false };
        bool betweenShots{ true };
    };
    std::array<Aimbot, 40> aimbot;

    struct Triggerbot {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        bool friendlyFire{ false };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        int hitgroup{ 0 };
        int shotDelay{ 0 };
        int minDamage{ 1 };
        bool killshot{ false };
    };
    std::array<Triggerbot, 40> triggerbot;

    struct {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
        int timeLimit{ 200 };
    } backtrack;

    struct {
        bool enabled{ false };
        bool pitch{ false };
        float pitchAngle{ 0.0f };
        bool yaw{ false };
    } antiAim;

    struct Glow {
        bool enabled{ false };
        bool healthBased{ false };
        bool rainbow{ false };
        float thickness{ 1.0f };
        float alpha{ 1.0f };
        int style{ 0 };
        float color[3]{ 1.0f, 1.0f, 1.0f };
    };
    std::array<Glow, 21> glow;

    struct Chams {
        bool enabled{ false };
        bool healthBased{ false };
        bool rainbow{ false };
        bool blinking{ false };
        int material{ 0 };
        bool wireframe{ false };
        float color[3]{ 1.0f, 1.0f, 1.0f };
        float alpha{ 1.0f };
    };

    std::array<Chams, 17> chams;

    struct Esp {
        bool enabled{ false };
        int font{ 0x1d };
        bool snaplines{ false };
        float snaplinesColor[3]{ 1.0f, 1.0f, 1.0f };
        bool eyeTraces{ false };
        float eyeTracesColor[3]{ 1.0f, 1.0f, 1.0f };
        bool box{ false };
        float boxColor[3]{ 1.0f, 1.0f, 1.0f };
        bool name{ false };
        float nameColor[3]{ 1.0f, 1.0f, 1.0f };
        bool health{ false };
        float healthColor[3]{ 1.0f, 1.0f, 1.0f };
        bool healthBar{ false };
        float healthBarColor[3]{ 1.0f, 1.0f, 1.0f };
        bool armor{ false };
        float armorColor[3]{ 1.0f, 1.0f, 1.0f };
        bool armorBar{ false };
        float armorBarColor[3]{ 1.0f, 1.0f, 1.0f };
        bool money{ false };
        float moneyColor[3]{ 1.0f, 1.0f, 1.0f };
        bool headDot{ false };
        float headDotColor[3]{ 1.0f, 1.0f, 1.0f };
    };

    std::array<Esp, 6> esp;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
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
        bool zoom{ false };
        int zoomKey{ 0 };
        bool thirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
        int viewmodelFov{ 0 };
        int fov{ 0 };
        int farZ{ 0 };
        int flashReduction{ 0 };
        float brightness{ 0.0f };
        int skybox{ 0 };
        float worldColor[3]{ 0.0f, 0.0f, 0.0f };
        bool deagleSpinner{ false };
        int screenEffect{ 0 };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
    } visuals;

    std::array<item_setting, 36> skinChanger;

    struct {
        int chickenVolume{ 100 };

        struct Player {
            int masterVolume{ 100 };
            int headshotVolume{ 100 };
            int weaponVolume{ 100 };
            int footstepVolume{ 100 };
        };

        std::array<Player, 3> players;
    } sound;

    struct {
        int menuStyle{ 0 };
        int menuColors{ 0 };
    } style;

    struct {
        int menuKey{ 0x2D }; // VK_INSERT
        bool antiAfkKick{ false };
        bool autoStrafe{ false };
        bool bunnyHop{ false };
        bool customClanTag{ false };
        char clanTag[16]{ "" };
        bool animatedClanTag{ false };
        bool fastDuck{ false };
        bool moonwalk{ false };
        bool sniperCrosshair{ false };
        bool recoilCrosshair{ false };
        bool autoPistol{ false };
        bool autoReload{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool revealRanks{ false };
        bool spectatorList{ false };
        bool watermark{ false };
        bool fixAnimationLOD{ false };
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        bool killMessage{ false };
        char killMessageString[230]{ "Gotcha!" };
        bool nameStealer{ false };
        char voteText[50]{ "" };
        int banColor{ 6 };
        char banText[150]{ "Cheater has been permanently banned from official CS:GO servers." };
        bool fastPlant{ false };
        bool bombTimer{ false };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
        int chokedPackets{ 0 };
        int chokedPacketsKey{ 0 };
        float maxAngleDelta{ 255.0f };
    } misc;

    struct {
        bool enabled{ false };
        int target{ 0 };
        int delay{ 10 };
        bool aimbot{ true };
        bool wallhack{ true };
        bool other{ true };
        bool griefing{ false };
        bool voiceAbuse{ false };
        bool textAbuse{ false };
    } reportbot;
private:
    std::filesystem::path path;
    std::vector<std::string> configs;
};

extern Config config;
