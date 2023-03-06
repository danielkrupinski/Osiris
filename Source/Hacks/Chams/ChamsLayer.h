#pragma once

#include <string_view>

#include "ChamsMaterial.h"
#include "ChamsMaterials.h"
#include <ConfigStructs.h>

struct ChamsLayer {
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

            for (std::uint8_t i = 0; i < ChamsMaterials::numberOfMaterials; ++i) {
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
