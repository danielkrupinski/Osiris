#pragma once

#include <unordered_map>

class Netvars final {
public:
    Netvars();
private:
    std::unordered_map<std::string, void*> recvTables;
};
