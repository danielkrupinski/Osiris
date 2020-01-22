#pragma once

#include <array>
#include <filesystem>
#include <string>

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

    struct Color {
        float color[3]{ 1.0f, 1.0f, 1.0f };
        bool rainbow{ false };
        float rainbowSpeed{ 0.6f };
    };
    
    struct ColorToggle : public Color {
        bool enabled{ false };
    };

    struct Aimbot {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        int keyMode{ 0 };
        bool aimlock{ false };
        bool silent{ false };
        bool friendlyFire{ false };
        bool visibleOnly{ false };
        bool scopedOnly{ false };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool autoScope{ false };
        bool recoilbasedFov{ false };
		int viewmodelFov{ 0 };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float recoilControlX{ 0.0f };
        float recoilControlY{ 0.0f };
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
        bool standaloneRCS{ false };
        int rcsStyle{ 0 };
        int minDamage{ 0 };
		int hitChance{ 0 };
		float shotsFired{ 0.0f };
        bool killshot{ false };
        bool betweenShots{ false };
		bool velocityExtrapolation{ false };
		bool aimbotCircle{ false };
    };
    std::array<Aimbot, 40> aimbot;

    struct Triggerbot {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        bool friendlyFire{ false };
        bool scopedOnly{ false };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        int hitgroup{ 0 };
        int shotDelay{ 0 };
        int minDamage{ 0 };
        bool killshot{ false };
    };
    std::array<Triggerbot, 40> triggerbot;

    struct {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
		bool drawAllTicks{ false };
        bool pingBased{ 0 };
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
        float thickness{ 1.0f };
        float alpha{ 1.0f };
        int style{ 0 };
        Color color;
    };
    std::array<Glow, 21> glow;

    struct Chams {
        struct Material {
            bool enabled{ false };
            bool healthBased{ false };
            Color color;
            bool blinking{ false };
            int material{ 0 };
            bool wireframe{ false };
            float alpha{ 1.0f };
        };
        std::array<Material, 2> materials;
    };

	std::array<Chams, 18> chams;

    struct Esp {
        struct Shared {
            bool enabled{ false };
            int font{ 0x1d };
            ColorToggle snaplines;
            ColorToggle box;
            int boxType{ 0 };
            ColorToggle name;
            ColorToggle outline{ 0.0f, 0.0f, 0.0f };
            ColorToggle distance;
			bool deadesp{ false };
            float maxDistance{ 0.0f };
        };
       
        struct Player : public Shared {
            ColorToggle eyeTraces;
            ColorToggle health;
            ColorToggle healthBar;
            ColorToggle armor;
            ColorToggle armorBar;
            ColorToggle money;
            ColorToggle headDot;
            ColorToggle activeWeapon;
        };

        struct Weapon : public Shared { } weapon;

        struct Projectile : public Shared { };
        std::array<Projectile, 9> projectiles;

        struct DangerZone : public Shared { };
        std::array<DangerZone, 18> dangerZone;

        std::array<Player, 6> players;
    } esp;

    struct {
        bool disablePostProcessing{ false };
		bool disablePanoramablur{ false };
        bool inverseRagdollGravity{ false };
        int inverseRagdollGravityValue{ -600 };
        bool ragdollTimescale{ false };
        int ragdollTimescaleValue{ 1 };
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
        bool noZoom{ false };
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
        bool fullBright{ false };
        int skybox{ 0 };
		ColorToggle world;
        ColorToggle sky;
        bool deagleSpinner{ false };
        int screenEffect{ 0 };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
        bool hitMarkerDamageIndicator{ false };
        bool hitMarkerDamageIndicatorCustomize{ false };
        int hitMarkerDamageIndicatorDist{ 150 };
        float hitMarkerDamageIndicatorRatio{ 0.0f };
        int hitMarkerDamageIndicatorAlpha{ 800 };
        int hitMarkerDamageIndicatorFont{ 31 };
        int hitMarkerDamageIndicatorTextX{ 60 };
        int hitMarkerDamageIndicatorTextY{ 150 };
		bool viewModel{ false };
		float viewModel_x{ 0 };
		float viewModel_y{ 0 };
		float viewModel_z{ 0 };
        bool viewModelKnife{ false };
        bool viewModelKnifeOut{ false };
        bool viewModelKnifeEnabled{ false };
        bool viewModelKnifeSwitch{ false };
        bool viewModelBombEquipped{ false };
        bool viewModelFlipKnifeHand{ false };
        float viewModel_x_Knife{ 0 };
        float viewModel_y_Knife{ 0 };
        float viewModel_z_Knife{ 0 };
        bool viewBob{ false };
        int playerModelT{ 0 };
        int playerModelCT{ 0 };

        struct {
            bool enabled = false;
            float blue = 0.0f;
            float red = 0.0f;
            float mono = 0.0f;
            float saturation = 0.0f;
            float ghost = 0.0f;
            float green = 0.0f;
            float yellow = 0.0f;
        } colorCorrection;
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
		bool bunnyHop{ false };
		bool autoStrafe{ false };
        int autoStrafeKey{ 0 };
        int autostrafestyle{ 0 };
        bool edgeJump{ false };
        int edgeJumpKey{ 0 };
        bool useSpam{ false };
        bool slowWalk{ false };
        int slowWalkKey{ 0 };
        bool customClanTag{ false };
        bool clocktag{ false };
        char clanTag[16]{ "" };
        bool animatedClanTag{ false };
        bool fastDuck{ false };
		int fakeDuckKey{ 0 };
        bool moonwalk{ false };
        bool sniperCrosshair{ false };
		bool sniperCrosshairInscope{ false };
        bool recoilCrosshair{ false };
        bool autoPistol{ false };
        bool autoReload{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool revealRanks{ false };
		bool revealMoney{ false };
		bool revealSuspect{ false };
        ColorToggle spectatorList;
        ColorToggle watermark;
        bool fixAnimationLOD{ false };
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        float aspectRatio{ 0 };
        bool chatSpam{ false };
        int chatSpamDelay{ 0 };
        char chatSpamText[8192]{ };
        bool randomChatSpam{ false };
        bool killMessage{ false };
        std::string killMessageString{ "" };
        bool nameStealer{ false };
        char voteText[50]{ "" };
        int banColor{ 6 };
        char banText[150]{ "<user> has been permanently banned from official CS:GO servers." };
        bool fastPlant{ false };
        ColorToggle bombTimer{ 1.0f, 0.55f, 0.0f };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
        int chokedPackets{ 0 };
        int chokedPacketsKey{ 0 };
        int quickHealthshotKey{ 0 };
        bool nadePredict{ false };
        bool fixTabletSignal{ false };
        float maxAngleDelta{ 255.0f };
        bool fakePrime{ false };
    } misc;

    struct {
        bool enabled{ false };
        int target{ 0 };
        int delay{ 10 };
        bool aimbot{ false };
        bool wallhack{ false };
        bool other{ false };
        bool griefing{ false };
        bool voiceAbuse{ false };
        bool textAbuse{ false };
    } reportbot;
private:
    std::filesystem::path path;
    std::vector<std::string> configs;
};

extern Config config;
