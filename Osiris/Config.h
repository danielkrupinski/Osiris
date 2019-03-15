#pragma once

#include <filesystem>
#include <string_view>

class Config final {
public:
    explicit Config(const char*) noexcept;
    void load() noexcept;
    void save() const noexcept;
    void reset() noexcept;

    struct {
        struct {
            bool enabled{ false };
            bool silent{ false };
            bool friendlyFire{ false };
            bool visibleOnly{ true };
            bool scopedOnly{ true };
            bool autoShot{ false };
            float fov{ 0.0f };
            float smooth{ 1.0f };
            int bone{ 0 };
            float recoilControlX{ 0.0f };
            float recoilControlY{ 0.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, silent, friendlyFire, visibleOnly,
                    scopedOnly, autoShot, fov, smooth,
                    bone, recoilControlX, recoilControlY);
            }
        } weapons[35];

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(weapons);
        }
    } aimbot;

    struct {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        int shotDelay{ 0 };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(enabled, onKey, key, shotDelay);
        }
    } triggerbot;

    struct {
        bool enabled{ false };
        float thickness{ 1.0f };
        float alpha{ 1.0f };
        int style{ 0 };
        bool allies{ false };
        float alliesColor[3]{ 0.0f, 1.0f, 0.0f };
        bool enemies{ false };
        float enemiesColor[3]{ 1.0f, 0.0f, 0.0f };
        bool localPlayer{ false };
        float localPlayerColor[3]{ 0.15f, 0.76f, 0.8f };
        bool weapons{ false };
        float weaponsColor[3]{ 1.0f, 1.0f, 1.0f };
        bool C4{ false };
        float C4Color[3]{ 1.0f, 1.0f, 0.0f };
        bool plantedC4{ false };
        float plantedC4Color[3]{ 1.0f, 0.27f, 0.0f };
        bool chickens{ false };
        float chickensColor[3]{ 1.0f, 0.08f, 0.58f };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(enabled, thickness, alpha, style, allies, alliesColor,
                enemies, enemiesColor, localPlayer, localPlayerColor,
                weapons, weaponsColor, C4, C4Color,
                plantedC4, plantedC4Color, chickens, chickensColor);
        }
    } glow;

    struct {
        struct {
            bool enabled{ false };
            bool flat{ false };
            bool wireframe{ false };
            float color[3]{ 1.0f, 1.0f, 1.0f };
            float alpha{ 1.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, flat, wireframe, color, alpha);
            }
        } chams[11];

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(chams);
        }
    } chams;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
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
        int skybox{ 0 };
        float worldColor[3]{ 0.0f, 0.0f, 0.0f };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(disablePostProcessing, inverseRagdollGravity, noVisualRecoil, noHands,
                noSleeves, noWeapons, noSmoke, noBlur, noScopeOverlay, noGrass, wireframeSmoke, thirdperson,
                thirdpersonKey, thirdpersonDistance, viewmodelFov, flashReduction, brightness, skybox,
                worldColor);
        }
    } visuals;

    struct {
        bool enabled{ false };
        int knife{ 0 };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(enabled, knife);
        }
    } knifeChanger;

    struct {
        bool enabled{ false };
    } skinChanger;

    struct {
        bool bunnyHop{ false };
        bool animatedClanTag{ false };
        bool fastDuck{ false };
        bool sniperCrosshair{ false };
        bool recoilCrosshair{ false };
        bool autoPistol{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool spectatorList{ false };
        bool watermark{ false };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(bunnyHop, animatedClanTag, fastDuck,
                sniperCrosshair, recoilCrosshair, autoPistol,
                autoAccept, radarHack, spectatorList, watermark);
        }
    } misc;

private:
    std::filesystem::path path;
};

extern Config config;
