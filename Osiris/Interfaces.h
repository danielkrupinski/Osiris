#pragma once

#include <string>

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);
};

extern Interfaces interfaces;
