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
            bool plantedC4{ false };
            float plantedC4Color[3]{ 1.0f, 0.27f, 0.0f };
            bool defuseKits{ false };
            float defuseKitsColor[3]{ 0.0f, 0.0f, 1.0f };
            bool chickens{ false };
            float chickensColor[3]{ 1.0f, 0.08f, 0.58f };
        } glow;
    } visuals;

    struct {
        bool bunnyHop{ false };
        bool disablePostProcessing{ false };
        std::string skybox{ xorstr_("Default") };
    } misc;

private:
    std::string path;
};

extern Config config;
