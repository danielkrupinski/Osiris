#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>

#include "imgui/imgui.h"
#include "nSkinz/config_.hpp"
#include "ConfigStructs.h"

#include "SDK/Vector.h"

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
	struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
    };

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
        bool killshot{ false };
        bool betweenShots{ true };
    };
    std::array<Aimbot, 40> aimbot;

	struct Ragebot {
		bool enabled{ false };
		bool onKey{ false };
		int key{ 0 };
		int keyMode{ 0 };
		bool slient{ false };
		bool betweenShots{ false };
		bool friendlyFire{ false };
		bool autoStop{ false };
		float bodyChance{ 0.0f };
		float pointChance{ 0.0f };
		float CanSeeDamage{ 0.0f };
        float WallDamage{ 0.0f };
		float hitChance{ 0.0f };
		bool BonesBools[8]{
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false
		};
	};
	std::array<Ragebot, 45> ragebot;

	const char* BonesTexts[8] = 
	{
			"Head",
			"Neck",
			"UpperChest",
			"Body",
			"Pelvis",
			"Hands",
			"Legs",
			"FeetS"
	};

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
    };
    std::array<Triggerbot, 40> triggerbot;

    struct Backtrack {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
		int timeLimit{ 0 };
        bool fakeLatency{ false };
    } backtrack;

    struct AntiAim {
        float pitchAngle{ 0.0f };
    	 struct {
            bool enabled{ false };
            int yawInverseAngleKey{ 0 };
            int yawInverseKeyMode{ 0 };
            bool yawInversed{ false };
            struct {
                bool enabled{ false };
                int key{ 0 };
                int keyMode{ 0 };
                bool keyToggled{ false };
                int maxChoke{ 3 };
            } fakeWalk;
        } general;

        struct {
            bool enabled{ false };
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
                struct {
                    int mode{ 0 };
                    float step{ 0.0f };
                    float jitterMax{ 0.0f };
                    float jitterMin{ 0.0f };
                } fake;
                struct {
                    bool enabled{ false };
                    float bodyLean{ 0.0f };
                    int mode{ 0 };
                    float step{ 0.0f };
                    float jitterMax{ 0.0f };
                    float jitterMin{ 0.0f };
                    struct {
                        bool enabled{ false };
                        float angle{ 0.0f };
                    } LBYBreaker;
                } desync;
            } yaw;
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
            } pitch;
        } standing;

        struct {
            bool enabled{ false };
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
                struct {
                    int mode{ 0 };
                    float step{ 0.0f };
                    float jitterMax{ 0.0f };
                    float jitterMin{ 0.0f };
                } fake;
                struct {
                    bool enabled{ false };
                    float bodyLean{ 0.0f };
                    int mode{ 0 };
                    float step{ 0.0f };
                    float jitterMax{ 0.0f };
                    float jitterMin{ 0.0f };
                    struct {
                        bool enabled{ false };
                        float angle{ 0.0f };
                    } LBYBreaker;
                } desync;
            } yaw;
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
            } pitch;
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
            } LBYBreaker;
        } moving;

        struct {
            bool enabled{ false };
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
                struct {
                    int mode{ 0 };
                    float step{ 0.0f };
                    float jitterMax{ 0.0f };
                    float jitterMin{ 0.0f };
                } fake;
                struct {
                    bool enabled{ false };
                    float bodyLean{ 0.0f };
                    int mode{ 0 };
                    float step{ 0.0f };
                    float jitterMax{ 0.0f };
                    float jitterMin{ 0.0f };
                    struct {
                        bool enabled{ false };
                        float angle{ 0.0f };
                    } LBYBreaker;
                } desync;
            } yaw;
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
            } pitch;
            struct {
                bool enabled{ false };
                float angle{ 0.0f };
            } LBYBreaker;
        } inAir;
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
        ColorToggle world;
        ColorToggle sky;
        bool deagleSpinner{ false };
        int screenEffect{ 0 };
        int hitEffect{ 0 };
        float hitEffectTime{ 0.6f };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
        int playerModelT{ 0 };
        int playerModelCT{ 0 };
        bool nightMode{ false };

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


          bool indicatorsEnabled{ false };
        const char* indicators[4] = {
            "Desync",
            "LBY",
            "Fakelag",
            "Fakeduck"
        };
        bool selectedIndicators[4] = {
            false,
            false,
            false,
            false
        };
    	
    	 ColorToggle bulletTracers;
    
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
    } style;

    struct Misc {
        int menuKey{ 0x2D }; // VK_INSERT
        bool antiAfkKick{ false };
        bool autoStrafe{ false };
        bool bunnyHop{ false };
        bool customClanTag{ false };
        bool clocktag{ false };
        char clanTag[16];
        bool animatedClanTag{ false };
        bool fastDuck{ false };
        bool moonwalk{ false };
        bool edgejump{ false };
        int edgejumpkey{ 0 };
        bool slowwalk{ false };
        int slowwalkKey{ 0 };
        ColorToggleThickness noscopeCrosshair;
        ColorToggleThickness recoilCrosshair;
        bool autoPistol{ false };
        bool autoReload{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool revealRanks{ false };
        bool revealMoney{ false };
        bool revealSuspect{ false };
        ImGuiStruct spectatorList{ false };
        ColorToggle watermark;
        bool fixAnimationLOD{ false };
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        float aspectratio{ 0 };
        bool killMessage{ false };
        std::string killMessageString{ "Gotcha!" };
        bool nameStealer{ false };
        bool disablePanoramablur{ false };
        int banColor{ 6 };
        std::string banText{ "Cheater has been permanently banned from official CS:GO servers." };
        bool fastPlant{ false };
        ColorToggle bombTimer{ 1.0f, 0.55f, 0.0f };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
         int fakeLagMode{ 0 };
        int fakeLagTicks{ 0 };
        int fakeLagKey{ 0 };
        const char* fakeLagFlags[4] = {
            "While Shooting",
            "While Standing",
            "While Moving",
            "In Air"
        };
        bool fakeLagSelectedFlags[4] = {
            false,
            false,
            false,
            false
        };
        int quickHealthshotKey{ 0 };
        bool nadePredict{ false };
        bool fixTabletSignal{ false };
        float maxAngleDelta{ 255.0f };
        bool fakePrime{ false };
    	 bool fakeDuck{ false };
        int fakeDuckKey{ 0 };
        bool fakeDucking{ false };
        int fakeDuckShotState{ 0 };
        int killSound{ 0 };
        bool drawInaccuracy{ false };
        float drawInaccuracyThickness{ 0.0f };
        std::string customKillSound;
        std::string customHitSound;
        PurchaseList purchaseList;
        StatusBar Sbar;
    } misc;

    struct Reportbot {
        bool enabled{ false };
        bool textAbuse{ false };
        bool griefing{ false };
        bool wallhack{ true };
        bool aimbot{ true };
        bool other{ true };
        int target{ 0 };
        int delay{ 1 };
        int rounds{ 1 };
    } reportbot;

	 struct {
        bool thirdPersonAnglesSet{ false };
        Vector fakeAngle;
        Vector realAngle;
        Vector cmdAngle;
        Record serverPos;
        float serverTime{ 0.f };
        int tickRate{ 0 };
        float nextLBY{ 0.f };
        float lastLBY{ 0.f };
        int chokedPackets{ 0 };
        bool sendPacket{ false };
    } globals;

    struct{
        Vector viewangles;

    }globalvars;

    void scheduleFontLoad(const std::string& name) noexcept;
    bool loadScheduledFonts() noexcept;
private:
    std::vector<std::string> scheduledFonts{ "Default" };
    std::filesystem::path path;
    std::vector<std::string> configs;
};

inline std::unique_ptr<Config> config;
