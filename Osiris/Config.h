#pragma once

#include <filesystem>
#include <string>

class Config final {
public:
    explicit Config(const std::string_view);
    void load();
    void save() const;
    void reset();

    struct {
        struct {
            bool enabled{ false };
            bool silent{ false };
            bool visibleOnly{ true };
            float fov{ 0.0f };
            float smooth{ 1.0f };
            int bone{ 0 };
            float recoilControlX{ 0.0f };
            float recoilControlY{ 0.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, silent, visibleOnly, fov, smooth,
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
                enemies, enemiesColor, weapons, weaponsColor, C4, C4Color,
                plantedC4, plantedC4Color, chickens, chickensColor);
        }
    } glow;

    struct {
        struct {
            bool enabled{ false };
            float alpha{ 1.0f };
            bool flat{ false };
            bool wireframe{ false };
            float color[3]{ 1.0f, 1.0f, 1.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, alpha, flat, wireframe, color);
            }
        } chams[6];

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
        bool wireframeSmoke{ false };
        bool thirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
        int viewmodelFov{ 0 };
        int flashReduction{ 0 };
        int skybox{ 0 };
        float worldColor[3]{ 0.0f, 0.0f, 0.0f };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(disablePostProcessing, inverseRagdollGravity, noVisualRecoil, noHands,
                noSleeves, noSmoke, noWeapons, wireframeSmoke, thirdperson, thirdpersonKey,
                thirdpersonDistance, viewmodelFov, flashReduction, skybox, worldColor);
        }
    } visuals;

    struct {
        bool bunnyHop{ false };
        bool animatedClanTag{ false };
        bool fastDuck{ false };
        bool autoPistol{ false };
        bool radarHack{ false };

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(bunnyHop, animatedClanTag, fastDuck, autoPistol, radarHack);
        }
    } misc;

private:
    std::filesystem::path path;
};

extern Config config;
