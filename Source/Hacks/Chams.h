#pragma once

#include <array>

#include "../Memory.h"

#include "Chams/ChamsCategory.h"
#include "Chams/ChamsMaterial.h"
#include "Chams/ChamsMaterialFactory.h"
#include <Helpers/KeyValuesFactory.h>
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/EngineInterfaces.h>
#include <Interfaces/OtherInterfaces.h>

#include <ConfigStructs.h>

namespace csgo
{

class Entity;
struct ModelRenderInfo;
class matrix3x4;
class StudioRender;
struct MaterialPOD;

}

class Backtrack;

class Chams {
public:
    Chams(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const PatternFinder& clientPatternFinder)
        : clientInterfaces{ clientInterfaces }, engineInterfaces{ engineInterfaces }, interfaces{ interfaces }, memory{ memory }, materialFactory{ ChamsMaterialKeyValuesFactory{ createKeyValuesFactory(retSpoofGadgets->client, clientPatternFinder) }, interfaces.getMaterialSystem() }
    {
    }

    bool render(Backtrack& backtrack, void*, void*, const csgo::ModelRenderInfo&, csgo::matrix3x4*) noexcept;
    void updateInput() noexcept;

    static constexpr auto numberOfMaterials = 13;
    static constexpr auto numberOfCategories = 9;

    struct Material {
        bool enabled = false;
        bool healthBased = false;
        bool blinking = false;
        bool wireframe = false;
        bool cover = false;
        bool ignorez = false;
        ChamsMaterial material = ChamsMaterial::Normal;
        Color4 color;

        template <typename Configurator>
        void configure(Configurator& configurator)
        {
            configurator("Enabled", enabled).def(false);
            configurator("Health based", healthBased).def(false);
            configurator("Blinking", blinking).def(false);
            configurator("Wireframe", wireframe).def(false);
            configurator("Cover", cover).def(false);
            configurator("Ignore-Z", ignorez).def(false);
            configurator("Material", material)
                .def(ChamsMaterial::Normal)
                .loadString([this](std::string_view str) {
                    if (str.empty())
                        return;

                    for (std::uint8_t i = 0; i < numberOfMaterials; ++i) {
                        if (toString(ChamsMaterial(i)) == str) {
                            material = ChamsMaterial(i);
                            break;
                        }
                    }
                })
                .save([this] { return toString(material); });
            configurator("Color", color);
        }
    };

    struct ChamsCategoryMaterials {
        std::array<Material, 7> materials;

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
    void initializeMaterials(const csgo::MaterialSystem& materialSystem) noexcept;
    void renderPlayer(Backtrack& backtrack, const csgo::Entity& player) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;

    bool appliedChams;
    void* ctx;
    void* state;
    const csgo::ModelRenderInfo* info;
    csgo::matrix3x4* customBoneToWorld;

    void applyChams(const std::array<Material, 7>& chams, int health = 0, csgo::matrix3x4* customMatrix = nullptr) noexcept;
    
    ClientInterfaces clientInterfaces;
    EngineInterfaces engineInterfaces;
    OtherInterfaces interfaces;
    const Memory& memory;
    ChamsMaterialFactory materialFactory;

    [[nodiscard]] csgo::Material getMaterial(ChamsMaterial material);

    std::array<csgo::MaterialPOD*, numberOfMaterials> materials{};
    std::array<bool, numberOfMaterials> materialsInitialized{};
};
