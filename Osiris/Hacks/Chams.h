#pragma once

#include "../SDK/ModelRender.h"

class Chams {
public:
    Chams() noexcept;
    void render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderWeapons() const noexcept;
    void renderHands() const noexcept;
private:
    Material* normal;
    Material* flat;
};
