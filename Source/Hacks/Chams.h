#pragma once

#include <array>

#include "../Config.h"
#include "../Memory.h"

namespace csgo
{

class Entity;
struct ModelRenderInfo;
class matrix3x4;
class StudioRender;

}

class ClientInterfaces;

class Chams {
public:
    bool render(Backtrack& backtrack, const csgo::Engine& engine, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Config& config, void*, void*, const csgo::ModelRenderInfo&, csgo::matrix3x4*) noexcept;
    static void updateInput(Config& config) noexcept;
private:
    void renderPlayer(Backtrack& backtrack, const csgo::Engine& engine, const csgo::StudioRender& studioRender, const Memory& memory, Config& config, const csgo::Entity& player) noexcept;
    void renderWeapons(const csgo::StudioRender& studioRender, const Memory& memory, Config& config) noexcept;
    void renderHands(const csgo::StudioRender& studioRender, const Memory& memory, Config& config) noexcept;
    void renderSleeves(const csgo::StudioRender& studioRender, const Memory& memory, Config& config) noexcept;

    bool appliedChams;
    void* ctx;
    void* state;
    const csgo::ModelRenderInfo* info;
    csgo::matrix3x4* customBoneToWorld;

    void applyChams(const csgo::StudioRender& studioRender, const Memory& memory, const std::array<Config::Chams::Material, 7>& chams, int health = 0, const csgo::matrix3x4* customMatrix = nullptr) noexcept;
};
