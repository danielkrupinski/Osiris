#pragma once

#include "../SDK/ModelRender.h"

namespace Chams {
    void renderDME(void*, void*, const ModelRenderInfo&, matrix3x4*);
    void renderWeapons(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
    void initialize();
};
