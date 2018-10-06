#pragma once

#include <string>

class Config final {
public:
    Config(const std::string&);
    void load();
    void save() const;

    struct {
        bool bunnyHop{ false };
        bool autoStrafe{ false };
    } misc;

private:
    std::string path;
};

extern Config config;
