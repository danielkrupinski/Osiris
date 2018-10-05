#pragma once

#include <string>

class Config final {
public:
    Config(const std::string&);
private:
    std::string path;
};