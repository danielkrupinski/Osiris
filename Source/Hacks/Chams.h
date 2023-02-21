#pragma once

#include <array>

#include "../Config.h"
#include "../Memory.h"

#include <Helpers/KeyValuesFactory.h>

namespace csgo
{

class Entity;
struct ModelRenderInfo;
class matrix3x4;
class StudioRender;

}

class ClientInterfaces;
class EngineInterfaces;
class OtherInterfaces;

class Chams {
public:
    Chams(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const PatternFinder& clientPatternFinder)
        : clientInterfaces{ clientInterfaces }, engineInterfaces{ engineInterfaces }, interfaces{ interfaces }, memory{ memory }, keyValuesFactory{ createKeyValuesFactory(retSpoofGadgets->client, clientPatternFinder) }
    {
    }

    bool render(Backtrack& backtrack, Config& config, void*, void*, const csgo::ModelRenderInfo&, csgo::matrix3x4*) noexcept;
    static void updateInput(Config& config) noexcept;
private:
    void initializeMaterials(const csgo::MaterialSystem& materialSystem) noexcept;
    void renderPlayer(Backtrack& backtrack, Config& config, const csgo::Entity& player) noexcept;
    void renderWeapons(Config& config) noexcept;
    void renderHands(Config& config) noexcept;
    void renderSleeves(Config& config) noexcept;

    bool appliedChams;
    void* ctx;
    void* state;
    const csgo::ModelRenderInfo* info;
    csgo::matrix3x4* customBoneToWorld;

    void applyChams(const std::array<Config::Chams::Material, 7>& chams, int health = 0, csgo::matrix3x4* customMatrix = nullptr) noexcept;
    
    ClientInterfaces clientInterfaces;
    EngineInterfaces engineInterfaces;
    OtherInterfaces interfaces;
    const Memory& memory;
    KeyValuesFactory keyValuesFactory;
};
