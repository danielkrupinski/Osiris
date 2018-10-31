#pragma once

#include <string>

#include "xorstr.hpp"

class Config final {
public:
    Config() = delete;
    explicit Config(const std::string&);
    void load();
    void save() const;
    void reset();

    struct {
        bool enabled{ false };
        float thickness{ 1.0f };
        float alpha{ 1.0f };
        int style{ 0 };
        bool players{ false };
        bool enemiesOnly{ false };
        float alliesColor[3]{ 0.0f, 1.0f, 0.0f };
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
        int reactionTime{ 0 };
    } triggerbot;

    struct {
        bool bunnyHop{ false };
        std::string clanTag;
        bool disablePostProcessing{ false };
        std::string skybox{ xorstr_("Default") };
        int viewmodelFov{ 60 };
    } misc;

private:
    std::string path;
};

extern Config config;
