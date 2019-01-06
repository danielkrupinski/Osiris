#pragma once

#include "../SDK/ModelRender.h"

namespace Chams {
    void render() noexcept;
    void renderDME(void*, void*, const ModelRenderInfo&, matrix3x4*);
};
