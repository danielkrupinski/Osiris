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
            float color[3]{ 1.0f, 1.0f, 1.0f };
            float thickness{ 1.0f };
            float alpha{ 1.0f };
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
