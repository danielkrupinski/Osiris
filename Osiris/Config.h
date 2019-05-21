#pragma once

#include <filesystem>

class Config final {
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

    struct {
        constexpr auto& operator[](size_t index) noexcept
        {
            return weapons[index];
        }
    private:
        struct {
            bool enabled{ false };
            bool onKey{ false };
            int key{ 0 };
            bool silent{ false };
            bool friendlyFire{ false };
            bool visibleOnly{ true };
            bool scopedOnly{ true };
            bool ignoreFlash{ false };
            bool ignoreSmoke{ false };
            bool autoShot{ false };
            bool recoilbasedFov{ false };
            float fov{ 0.0f };
            float maxAngleDelta{ 0.0f };
            float smooth{ 1.0f };
            int bone{ 0 };
            float recoilControlX{ 0.0f };
            float recoilControlY{ 0.0f };
        } weapons[35];
    } aimbot;

    struct {
        constexpr auto& operator[](size_t index) noexcept
        {
            return weapons[index];
        }
    private:
        struct {
            bool enabled{ false };
            bool onKey{ false };
            int key{ 0 };
            bool friendlyFire{ false };
            bool scopedOnly{ true };
            bool ignoreFlash{ false };
            bool ignoreSmoke{ false };
            int hitgroup{ 0 };
            int shotDelay{ 0 };
        } weapons[35];
    } triggerbot;

    struct {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        int timeLimit{ 200 };
    } backtrack;

    struct {
        constexpr auto& operator[](size_t index) noexcept
        {
            return glow[index];
        }
    private:
        struct {
            bool enabled{ false };
            bool healthBased{ false };
            bool rainbow{ false };
            float thickness{ 1.0f };
            float alpha{ 1.0f };
            int style{ 0 };
            float color[3]{ 1.0f, 1.0f, 1.0f };
        } glow[13];
    } glow;

    struct {
        constexpr auto& operator[](size_t index) noexcept
        {
            return chams[index];
        }
    private:
        struct {
            bool enabled{ false };
            bool healthBased{ false };
            bool rainbow{ false };
            int material{ 0 };
            bool wireframe{ false };
            float color[3]{ 1.0f, 1.0f, 1.0f };
            float alpha{ 1.0f };
        } chams[12];
    } chams;

    struct {
        constexpr auto& operator[](size_t index) noexcept
        {
            return esp[index];
        }
    private:
        struct {
            bool enabled{ false };
            bool snaplines{ false };
            float snaplinesColor[3]{ 1.0f, 1.0f, 1.0f };
            bool box{ false };
            float boxColor[3]{ 1.0f, 1.0f, 1.0f };
            bool name{ false };
            float nameColor[3]{ 1.0f, 1.0f, 1.0f };
            bool headDot{ false };
            float headDotColor[3]{ 1.0f, 1.0f, 1.0f };
        } esp[4];
    } esp;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
        bool noFog{ false };
        bool no3dSky{ false };
        bool noVisualRecoil{ false };
        bool noHands{ false };
        bool noSleeves{ false };
        bool noWeapons{ false };
        bool noSmoke{ false };
        bool noBlur{ false };
        bool noScopeOverlay{ false };
        bool noGrass{ false };
        bool wireframeSmoke{ false };
        bool thirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
        int viewmodelFov{ 0 };
        int flashReduction{ 0 };
        float brightness{ 0.0f };
        int skybox{ -1 };
        float worldColor[3]{ 0.0f, 0.0f, 0.0f };
    } visuals;

    struct {
        bool enabled{ false };
        int knife{ 0 };
    } knifeChanger;

    struct {
        constexpr auto& operator[](size_t index) noexcept
        {
            return weapons[index];
        }
    private:
        struct {
            bool enabled{ false };
            int paintKit{ 0 };
            float wear{ 0.0f };
        } weapons[49];
    } skinChanger;

    struct {
        int menuKey{ 0x2D }; // VK_INSERT
        bool autoStrafe{ false };
        bool bunnyHop{ false };
        bool animatedClanTag{ false };
        bool fastDuck{ false };
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
        bool prepareRevolver{ false };
        int chokedPackets{ 0 };
    } misc;

private:
    std::filesystem::path path;
    std::vector<std::string> configs;
};

extern Config config;
