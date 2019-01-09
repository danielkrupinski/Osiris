#pragma once

#include "../SDK/ModelRender.h"

class Chams {
public:
    Chams() noexcept;
    void render(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
    void renderDME(void*, void*, const ModelRenderInfo&, matrix3x4*);
    void renderWeapons(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
    void initialize();
private:
    Material* normal;
    Material* flat;
};
