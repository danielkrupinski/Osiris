#pragma once

#include <string>

class Config final {
public:
    explicit Config(const std::wstring&);
    void load();
    void save() const;
    void reset();

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
        bool defuseKits{ false };
        float defuseKitsColor[3]{ 0.0f, 0.0f, 1.0f };
        bool chickens{ false };
        float chickensColor[3]{ 1.0f, 0.08f, 0.58f };
    } glow;

    struct {
        bool enabled{ false };
        float alpha{ 1.0f };
        bool enemiesOnly{ false };
        bool visibleOnly{ false };
        float visibleAlliesColor[3]{ 0.0f, 1.0f, 0.0f };
        float occludedAlliesColor[3]{ 0.0f, 1.0f, 0.0f };
        float visibleEnemiesColor[3]{ 1.0f, 0.0f, 0.0f };
        float occludedEnemiesColor[3]{ 1.0f, 0.0f, 0.0f };
        bool wireframe{ false };
    } chams;

    struct {
        bool enabled{ false };
        int shotDelay{ 0 };
    } triggerbot;

    struct {
        bool bunnyHop{ false };
        std::string clanTag;
        bool disablePostProcessing{ false };
        int flashReduction{ 0 };
        bool inverseRagdollGravity{ false };
        bool noCrouchCooldown{ false };
        bool noBlood{ false };
        bool noSmoke{ false };
        bool wireframeSmoke{ false };
        std::string skybox{ "Default" };
        int viewmodelFov{ 60 };
        float worldColor[3]{ 0.0f, 0.0f, 0.0f };
    } misc;

private:
    std::string path;
};

extern Config config;
