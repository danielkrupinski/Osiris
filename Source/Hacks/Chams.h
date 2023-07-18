#pragma once

#include <array>

#include "../Memory.h"

#include "Chams/ChamsCategory.h"
#include "Chams/ChamsLayer.h"
#include "Chams/ChamsMaterial.h"
#include "Chams/ChamsMaterials.h"
#include "Chams/ChamsMaterialFactory.h"
#include <Helpers/KeyValuesFactory.h>
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/EngineInterfaces.h>
#include <Interfaces/OtherInterfaces.h>

#include <ConfigStructs.h>

namespace csgo
{

struct Entity;
struct ModelRenderInfo;
struct matrix3x4;
struct StudioRender;
struct MaterialPOD;

}

class Backtrack;
class ModelRenderHooks;

class Chams {
public:
    Chams(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const ClientPatternFinder& clientPatternFinder)
        : clientInterfaces{ clientInterfaces }, engineInterfaces{ engineInterfaces }, interfaces{ interfaces }, memory{ memory }, materials{ ChamsMaterialFactory{ ChamsMaterialKeyValuesFactory{ createKeyValuesFactory(retSpoofGadgets->client, clientPatternFinder) }, interfaces.getMaterialSystem() } }
    {
    }

    bool render(Backtrack& backtrack, void*, void*, const csgo::ModelRenderInfo&, csgo::matrix3x4*) noexcept;
    void updateInput() noexcept;

    void setModelRenderHooks(const ModelRenderHooks* modelRenderHooks) noexcept
    {
        this->modelRenderHooks = modelRenderHooks;
    }

    static constexpr auto numberOfCategories = 9;

    struct ChamsCategoryMaterials {
        std::array<ChamsLayer, 7> materials;

        template <typename Configurator>
        void configure(Configurator& configurator)
        {       
            configurator("Materials", materials);
        } 
    };

    std::array<ChamsCategoryMaterials, numberOfCategories> chamsMaterials;
    KeyBindToggle toggleKey;
    KeyBind holdKey;

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        for (std::size_t i = 0; i < chamsMaterials.size(); ++i)
            configurator(toString(static_cast<ChamsCategory>(i)).data(), chamsMaterials[i]);

        configurator("Toggle Key", toggleKey);
        configurator("Hold Key", holdKey);
    }

private:
    void renderPlayer(Backtrack& backtrack, const csgo::Entity& player) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;

    bool appliedChams;
    void* ctx;
    void* state;
    const csgo::ModelRenderInfo* info;
    csgo::matrix3x4* customBoneToWorld;

    void applyChams(const std::array<ChamsLayer, 7>& chams, int health = 0, csgo::matrix3x4* customMatrix = nullptr) noexcept;
    
    ClientInterfaces clientInterfaces;
    EngineInterfaces engineInterfaces;
    OtherInterfaces interfaces;
    const Memory& memory;
    ChamsMaterials materials;
    const ModelRenderHooks* modelRenderHooks = nullptr;
};
