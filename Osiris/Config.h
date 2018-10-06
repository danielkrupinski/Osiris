#pragma once

#include <string>

class Config final {
public:
    Config(const std::string&);
    void load();
    void save();

    int testValue;
private:
    std::string path;
};

extern Config config;
