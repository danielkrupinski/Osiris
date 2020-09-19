#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>

#include "imgui/imgui.h"
#include "nSkinz/config_.hpp"
#include "ConfigStructs.h"

class Config {
public:
    explicit Config(const char*) noexcept;
    void load(size_t, bool incremental) noexcept;
    void save(size_t) const noexcept;
    void add(const char*) noexcept;
    void remove(size_t) noexcept;
    void rename(size_t, const char*) noexcept;
    void reset() noexcept;
    void listConfigs() noexcept;

    constexpr auto& getConfigs() noexcept
    {
        return configs;
    }

    struct Color {
        std::array<float, 3> color{ 1.0f, 1.0f, 1.0f };
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
        bool visibleOnly{ true };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool autoScope{ false };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
        int minDamage{ 1 };
        float killDelay{ 0 };
        bool killshot{ false };
        bool jumpCheck{ false };
        bool betweenShots{ true };
        float recoilControlX{ 0.0f };
        float recoilControlY{ 0.0f };
        int shotsFired{ 0 };
        bool standaloneRCS{ false };
        bool randomRCS{ true };
    };
    std::array<Aimbot, 40> aimbot;

    struct Triggerbot {
        bool enabled = false;
        bool friendlyFire = false;
        bool scopedOnly = true;
        bool ignoreFlash = false;
        bool ignoreSmoke = false;
        bool killshot = false;
        bool onKey = false;
        int key = 0;
        int hitgroup = 0;
        int shotDelay = 0;
        int minDamage = 1;
        float burstTime = 0.0f;
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
    };
    std::array<Triggerbot, 40> triggerbot;

    struct Backtrack {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
        bool drawAllTicks{ false };
        bool pingBased{ false };
        int pingBasedVal{ 0 };
        int timeLimit{ 200 };
        bool fakeLatency{ false };
        int fakeLatencyAmmount{ 200 };
    } backtrack;

    struct AntiAim {
        bool enabled{ false };
        bool pitch{ false };
        bool yaw{ false };
        float pitchAngle{ 0.0f };
    } antiAim;

    struct Glow : ColorA {
        bool enabled{ false };
        bool healthBased{ false };
        int style{ 0 };
    };
    std::array<Glow, 21> glow;

    struct Chams {
        struct Material : ColorA {
            bool enabled = false;
            bool healthBased = false;
            bool blinking = false;
            bool wireframe = false;
            bool cover = false;
            bool ignorez = false;
            int material = 0;
        };
        std::array<Material, 7> materials;
    };

    std::unordered_map<std::string, Chams> chams;

    struct StreamProofESP {
        std::unordered_map<std::string, Player> allies;
        std::unordered_map<std::string, Player> enemies;
        std::unordered_map<std::string, Weapon> weapons;
        std::unordered_map<std::string, Projectile> projectiles;
        std::unordered_map<std::string, Shared> lootCrates;
        std::unordered_map<std::string, Shared> otherEntities;
    } streamProofESP;

    struct Font {
        ImFont* tiny;
        ImFont* medium;
        ImFont* big;
    };

    std::vector<std::string> systemFonts{ "Default" };
    std::unordered_map<std::string, Font> fonts;

    struct Visuals {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
        bool inverseRagdollGravityCustomize{ false };
        int inverseRagdollGravityValue{ -600 };
        bool ragdollTimescale{ false };
        float ragdollTimescaleValue{ 1.0f };
        bool nightMode{ false };
        bool noFog{ false };
        bool no3dSky{ false };
        bool noAimPunch{ false };
        bool noViewPunch{ false };
        bool noHands{ false };
        bool noSleeves{ false };
        bool noWeapons{ false };
        bool noSmoke{ false };
        bool noBlur{ false };
        bool noBloom{ false };
        bool noScopeOverlay{ false };
        bool noGrass{ false };
        bool noShadows{ false };
        bool wireframeSmoke{ false };
        ColorToggle showvelocity;
        bool showvelocityM{ false };
        int showvelocityPosX{ 0 };
        int showvelocityPosY{ 0 };
        int showvelocityResX;
        int showvelocityResY;
        bool zoom{ false };
        int zoomKey{ 0 };
        bool noZoom{ false };
        bool thirdperson{ false };
        bool deadThirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
        int viewmodelFov{ 0 };
        int fov{ 0 };
        int farZ{ 0 };
        int flashReduction{ 0 };
        float brightness{ 0.0f };
        bool fullBright{ false };
        int skybox{ 0 };
        std::string customSkybox;
        ColorToggleThickness rainbowBar;
        bool rainbowUp{ true };
        bool rainbowBottom{ false };
        bool rainbowLeft{ false };
        bool rainbowRight{ false };
        float rainbowScale{ 0.125f };
        bool rainbowPulse{ false };
        float rainbowPulseSpeed{ 1.0f };
        ColorToggle world;
        ColorToggle sky;
        bool deagleSpinner{ false };
        int screenEffect{ 0 };
        int hitEffect{ 0 };
        float hitEffectTime{ 0.6f };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
        ColorToggle hitMarkerDamageIndicator;
        bool hitMarkerDamageIndicatorCustomize{ false };
        int hitMarkerDamageIndicatorFont{ 31 };
        int hitMarkerDamageIndicatorAlpha{ 800 };
        int hitMarkerDamageIndicatorDist{ 150 };
        int hitMarkerDamageIndicatorTextX{ 60 };
        int hitMarkerDamageIndicatorTextY{ 150 };
        float hitMarkerDamageIndicatorRatio{ 0.0f };
        int playerModelT{ 0 };
        int playerModelCT{ 0 };
        ColorToggle bulletTracers;

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
    } visuals;

    std::array<item_setting, 36> skinChanger;

    struct Sound {
        int chickenVolume{ 100 };

        struct Player {
            int masterVolume{ 100 };
            int headshotVolume{ 100 };
            int weaponVolume{ 100 };
            int footstepVolume{ 100 };
        };

        std::array<Player, 3> players;
    } sound;

    struct Style {
        int menuStyle{ 0 };
        int menuColors{ 0 };
        struct StyleCustomEasy {
            ColorA BackGroundColor;
            ColorA TextColor;
            ColorA MainColor;
            ColorA MainAccentColor;
            ColorA HighlightColor;
        } customEasy;
    } style;

    struct Misc {
        int menuKey{ 0x2D }; // VK_INSERT
        bool antiAfkKick{ false };
        bool autoStrafe{ false };
        int autoStrafeKey{ 0 };
        int autoStrafeStyle{ 0 };
        bool bunnyHop{ false };
        bool bhopHitchanceEnable{ false };
        int bhopHitchance{ 100 };
        int bhopMinHits{ 20 };
        int bhopMaxHits{ 20 };
        int clanTagStyle{ 0 };
        bool animatedClanTag{ false };
        std::string customMultiClanTag;
        float customClanTagSpeed{ 0.6f };
        float OsirisBETAClanTagSpeed{ 1.0f };
        float customMultiClanTagSpeed{ 1.0f };
        bool chatSpam{ false };
        bool chatSpamRandom{ false };
        int chatSpamDelay{ 1 };
        std::string chatSpamPhrases{ "MLP is the best\ndiscord.gg/xWEtQAn" };
        bool fastDuck{ false };
        bool moonwalk{ false };
        bool edgejump{ false };
        bool jumpbug{ false };
        int jumpbugkey{ 0 };
        bool jumpbughold{ false };
        bool slowwalk{ false };
        bool doorSpam{ false };
        bool buyBot{ false };
        int buyBotPrimary{ 0 };
        int buyBotSecondary{ 0 };
        bool buyBotVest{ false };
        bool buyBotVestHelm{ false };
        bool buyBotTaser{ false };
        bool buyBotDefuser{ false };
        bool buyBotMolotov{ false };
        bool buyBotDecoy{ false };
        bool buyBotFlashbang{ false };
        bool buyBotFlashbangX2{ false };
        bool buyBotHE{ false };
        bool buyBotSmoke{ false };
        bool autoPistol{ false };
        bool autoReload{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool revealRanks{ false };
        bool revealMoney{ false };
        bool revealSuspect{ false };
        bool fixAnimationLOD{ false };
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        bool nameStealer{ false };
        bool disablePanoramablur{ false };
        bool killMessage{ false };
        bool nadePredict{ false };
        bool fixTabletSignal{ false };
        bool fakePrime{ false };
        bool autoZeus{ false };
        int autoZeusMaxPenDist{ 0 };
        std::string customName;
        bool fakeItem{ false };
        bool fakeItemFlagsST{ false };
        bool fakeItemFlagsStar{ false };
        int fakeItemTeam{ 0 };
        int fakeItemMessageType{ 0 };
        int fakeItemType{ 0 };
        int fakeItemRarity{ 0 };
        int fakeItemPlayerColor{ 0 };
        std::string fakeItemPlayerName{ "" };
        std::string fakeItemName{ "" };
        bool fastPlant{ false };
        bool bombDamage{ false };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        bool customViewmodelToggle{ false };
        float viewmodel_x{ 0 };
        float viewmodel_y{ 0 };
        float viewmodel_z{ 0 };
        bool customViewmodelKnifeToggle{ false };
        bool customViewmodelSwitchHand{ false };
        int customViewmodelSwitchHandBind{ 0 };
        float viewmodel_x_knife{ 0 };
        float viewmodel_y_knife{ 0 };
        float viewmodel_z_knife{ 0 };
        bool view_bob{ false };
        bool oppositeHandKnife = false;
        int oppositeHandKnifeBind{ 0 };
        PreserveKillfeed preserveKillfeed;
        char clanTag[16];
        int edgejumpkey{ 0 };
        int slowwalkKey{ 0 };
        ColorToggleThickness noscopeCrosshair;
        ColorToggleThickness recoilCrosshair;
        ColorToggle spectatorList;
        ColorToggle watermark;
        bool watermarkNickname{ true };
        bool watermarkUsername{ true };
        bool watermarkFPS{ true };
        bool watermarkPing{ true };
        bool watermarkTickrate{ true };
        bool watermarkTime{ true };
        float watermarkPosX{ 0 };
        float watermarkPosY{ 0 };
        float watermarkScale{ 1.0f };
        float aspectratio{ 0 };
        std::string killMessageString{ "Gotcha!" };
        int banColor{ 6 };
        std::string banText{ "Cheater has been permanently banned from official CS:GO servers." };
        ColorToggle bombTimer{ 1.0f, 0.55f, 0.0f };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
        int chokedPackets{ 0 };
        bool chokedPacketsShooting{ false };
        bool chokedPacketsStanding{ false };
        bool chokedPacketsMoving{ false };
        bool chokedPacketsAir{ false };
        int chokedPacketsTicks{ 0 };
        int chokedPacketsKey{ 0 };
        bool pingBasedChoked{ false };
        int pingBasedChokedVal{ 0 };
        int quickHealthshotKey{ 0 };
        float maxAngleDelta{ 255.0f };
        bool svpurebypass{ false };
        bool svpurebypassOLD{ false };
        bool drawAimbotFov{ false };
        float actualFov{ 0.0f };
        bool teamDamageCounter{ false };
        bool playerBlocker{ false };
        int playerBlockerKey{ 0 };
        ColorToggleThickness drawInaccuracy;
        int killSound{ 0 };
        std::string customKillSound;
        std::string customHitSound;
        bool fakeDuck{ false };
        int fakeDuckKey{ 0 };
        bool fakeDucking{ false };
        PurchaseList purchaseList;

        struct Reportbot {
            bool enabled = false;
            bool textAbuse = false;
            bool griefing = false;
            bool wallhack = true;
            bool aimbot = true;
            bool other = true;
            int target = 0;
            int delay = 1;
            int rounds = 1;
        } reportbot;
    } misc;

    void scheduleFontLoad(const std::string& name) noexcept;
    bool loadScheduledFonts() noexcept;
private:
    std::vector<std::string> scheduledFonts{ "Default" };
    std::filesystem::path path;
    std::vector<std::string> configs;
};

inline std::unique_ptr<Config> config;
