#pragma once

#include <string>

class Config final {
public:
    explicit Config(const std::string&);
    void load();
    void save() const;

    struct {
        struct {
            bool enabled{ false };
            bool enemiesOnly{ false };
            float alliesColor[3]{ 1.0f, 1.0f, 1.0f };
            float enemiesColor[3]{ 1.0f, 1.0f, 1.0f };
            float thickness{ 1.0f };
            float alpha{ 1.0f };
        } glow;
    } visuals;

    struct {
        bool bunnyHop{ false };
    } misc;

private:
    std::string path;
};

extern Config config;
