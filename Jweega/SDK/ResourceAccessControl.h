#pragma once

#include <cstddef>

class ResourceAccessControl {
private:
    std::byte pad[152];
public:
    int accessingThreadCount;
};
