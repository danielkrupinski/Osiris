#pragma once

#include <cstddef>

class Material;

class StudioRender {
    std::byte pad[0x250];
public:
    Material* materialOverride;
};
