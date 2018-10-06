#pragma once

#include <string>

class Config final {
public:
    Config(const std::string&);
    void load();
    void save();

    int testValue;

    struct {
        bool bunnyHop;
        bool autoStrafe;
    } misc;

private:
    std::string path;
};

extern Config config;
