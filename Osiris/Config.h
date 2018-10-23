#pragma once

#include <string>

#include "xorstr.hpp"

class Config final {
public:
    explicit Config(const std::string&);
    void load();
    void save() const;
    void reset();

    struct {
        struct {
            bool enabled{ false };
            bool enemiesOnly{ false };
            float alliesColor[3]{ 0.0f, 1.0f, 0.0f };
            float enemiesColor[3]{ 1.0f, 0.0f, 0.0f };
            float thickness{ 1.0f };
            float alpha{ 1.0f };
            int style{ 0 };
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
