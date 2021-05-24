#pragma once

#include <array>

#include "../Config.h"

class Entity;
struct ModelRenderInfo;
class matrix3x4;

class Chams {
public:
    bool render(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
    static void updateInput() noexcept;
private:
    void renderPlayer(Entity* player) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;

    bool appliedChams;
    void* ctx;
    void* state;
    const ModelRenderInfo* info;
    matrix3x4* customBoneToWorld;

    void applyChams(const std::array<Config::Chams::Material, 7>& chams, int health = 0, const matrix3x4* customMatrix = nullptr) noexcept;
};
