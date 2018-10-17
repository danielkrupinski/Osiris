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
            float thickness{ 0.0f };
        } glow;
    } visuals;

    struct {
        bool bunnyHop{ false };
        bool autoStrafe{ false };
    } misc;

private:
    std::string path;
};

extern Config config;
