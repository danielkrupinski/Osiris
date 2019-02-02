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
            float fov{ 0.0f };
            float smooth{ 1.0f };
            int bone{ 0 };

            template <class Archive>
            void serialize(Archive& archive)
            {
                archive(enabled, silent, fov, smooth, bone);
            }
        } weapons[35];

        template <class Archive>
        void serialize(Archive& archive)
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
        void serialize(Archive& archive)
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
        void serialize(Archive& archive)
        {
            archive(enabled, thickness, alpha, style, allies, alliesColor,
                enemies, enemiesColor, weapons, weaponsColor, C4, C4Color,
                plantedC4, plantedC4Color, chickens, chickensColor);
        }
    } glow;

    struct {
        bool enabled{ false };
        float alpha{ 1.0f };
        bool flat{ false };
        bool wireframe{ false };
        bool visibleAllies{ false };
        float visibleAlliesColor[3]{ 0.0f, 1.0f, 0.0f };
        bool occludedAllies{ false };
        float occludedAlliesColor[3]{ 0.0f, 1.0f, 0.0f };
        bool visibleEnemies{ false };
        float visibleEnemiesColor[3]{ 1.0f, 0.0f, 0.0f };
        bool occludedEnemies{ false };
        float occludedEnemiesColor[3]{ 1.0f, 0.0f, 0.0f };
        bool weapons{ false };
        float weaponsColor[3]{ 0.12f, 0.33f, 0.62f };
        bool hands{ false };
        float handsColor[3]{ 0.05f, 0.61f, 0.47f };

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(enabled, alpha, flat, wireframe,
                visibleAllies, visibleAlliesColor,
                occludedAllies, occludedAlliesColor,
                visibleEnemies, visibleEnemiesColor,
                occludedEnemies, occludedEnemiesColor,
                weapons, weaponsColor, hands, handsColor);
        }
    } chams;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
        bool noVisualRecoil{ false };
        bool noHands{ false };
        bool noSleeves{ false };
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
        void serialize(Archive& archive)
        {
            archive(disablePostProcessing, inverseRagdollGravity, noVisualRecoil, noHands,
                noSleeves, noSmoke, wireframeSmoke, thirdperson, thirdpersonKey,
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
        void serialize(Archive& archive)
        {
            archive(bunnyHop, animatedClanTag, fastDuck, autoPistol, radarHack);
        }
    } misc;

private:
    std::filesystem::path path;
};

extern Config config;
