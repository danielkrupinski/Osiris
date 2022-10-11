#pragma once

#include <array>

#include "../Config.h"
#include "../Memory.h"

class Entity;
struct ModelRenderInfo;
class matrix3x4;

class Chams {
public:
    bool render(const Engine& engine, const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory, Config& config, void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
    static void updateInput(Config& config) noexcept;
private:
    void renderPlayer(const Engine& engine, const Interfaces& interfaces, const Memory& memory, Config& config, Entity* player) noexcept;
    void renderWeapons(const Interfaces& interfaces, const Memory& memory, Config& config) noexcept;
    void renderHands(const Interfaces& interfaces, const Memory& memory, Config& config) noexcept;
    void renderSleeves(const Interfaces& interfaces, const Memory& memory, Config& config) noexcept;

    bool appliedChams;
    void* ctx;
    void* state;
    const ModelRenderInfo* info;
    matrix3x4* customBoneToWorld;

    void applyChams(const Interfaces& interfaces, const Memory& memory, const std::array<Config::Chams::Material, 7>& chams, int health = 0, const matrix3x4* customMatrix = nullptr) noexcept;
};
