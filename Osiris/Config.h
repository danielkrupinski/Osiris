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
        struct {
            bool enabled{ false };
            bool silent{ false };
            bool friendlyFire{ false };
            bool visibleOnly{ true };
            bool scopedOnly{ true };
            bool ignoreFlash{ false };
            bool ignoreSmoke{ false };
            bool autoShot{ false };
            bool recoilbasedFov{ false };
            float fov{ 0.0f };
            float smooth{ 1.0f };
            int bone{ 0 };
            float recoilControlX{ 0.0f };
            float recoilControlY{ 0.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, silent, friendlyFire, visibleOnly,
                    scopedOnly, ignoreFlash, ignoreSmoke, autoShot,
                    recoilbasedFov, fov, smooth, bone, recoilControlX, recoilControlY);
            }
        } weapons[35];

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(weapons);
        }
    } aimbot;

    struct {
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

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, onKey, key, friendlyFire, scopedOnly,
                    ignoreFlash, ignoreSmoke, hitgroup, shotDelay);
            }
        } weapons[35];

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(weapons);
        }
    } triggerbot;

    struct {
        struct {
            bool enabled{ false };
            float thickness{ 1.0f };
            float alpha{ 1.0f };
            int style{ 0 };
            float color[3]{ 1.0f, 1.0f, 1.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, thickness, alpha, style, color);
            }
        } glow[13];

        constexpr auto& operator[](size_t index) noexcept
        {
            return glow[index];
        }

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(glow);
        }
    } glow;

    struct {
        struct {
            bool enabled{ false };
            int material{ 0 };
            bool wireframe{ false };
            float color[3]{ 1.0f, 1.0f, 1.0f };
            float alpha{ 1.0f };

            template <class Archive>
            constexpr void serialize(Archive& archive) noexcept
            {
                archive(enabled, material, wireframe, color, alpha);
            }
        } chams[11];

        constexpr auto& operator[](size_t index) noexcept
        {
            return chams[index];
        }

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(chams);
        }
    } chams;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
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

        template <class Archive>
        constexpr void serialize(Archive& archive) noexcept
        {
            archive(disablePostProcessing, inverseRagdollGravity, no3dSky, noVisualRecoil, noHands,
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
        bool autoStrafe{ false };
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
            archive(autoStrafe, bunnyHop, animatedClanTag, fastDuck,
                sniperCrosshair, recoilCrosshair, autoPistol,
                autoAccept, radarHack, spectatorList, watermark);
        }
    } misc;

private:
    std::filesystem::path path;
    std::vector<std::string> configs;
};

extern Config config;
