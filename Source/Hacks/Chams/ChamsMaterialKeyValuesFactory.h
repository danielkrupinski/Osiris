#pragma once

#include <CSGO/KeyValues.h>
#include <Helpers/KeyValuesFactory.h>

class ChamsMaterialKeyValuesFactory {
public:
    explicit ChamsMaterialKeyValuesFactory(const KeyValuesFactory& keyValuesFactory)
        : keyValuesFactory{ keyValuesFactory }
    {
    }

    [[nodiscard]] auto normal()
    {
        return keyValuesFactory("VertexLitGeneric", nullptr);
    }

    [[nodiscard]] auto flat()
    {
        return keyValuesFactory("UnlitGeneric", nullptr);
    }

    [[nodiscard]] auto chrome()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$envmap", "env_cubemap");
        return kv;
    }

    [[nodiscard]] auto glow()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$additive", "1");
        kv.setString("$envmap", "models/effects/cube_white");
        kv.setString("$envmapfresnel", "1");
        kv.setString("$alpha", ".8");
        return kv;
    }

    [[nodiscard]] auto pearlescent()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$ambientonly", "1");
        kv.setString("$phong", "1");
        kv.setString("$pearlescent", "3");
        kv.setString("$basemapalphaphongmask", "1");
        return kv;
    }

    [[nodiscard]] auto metallic()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$basetexture", "white");
        kv.setString("$ignorez", "0");
        kv.setString("$envmap", "env_cubemap");
        kv.setString("$normalmapalphaenvmapmask", "1");
        kv.setString("$envmapcontrast", "1");
        kv.setString("$nofog", "1");
        kv.setString("$model", "1");
        kv.setString("$nocull", "0");
        kv.setString("$selfillum", "1");
        kv.setString("$halfambert", "1");
        kv.setString("$znearer", "0");
        kv.setString("$flat", "1");
        return kv;
    }

    [[nodiscard]] auto animated()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapcontrast", "1");
        kv.setString("$basetexture", "dev/zone_warning");

        const auto textureScroll = kv.findKey("proxies", true).findKey("texturescroll", true);
        textureScroll.setString("texturescrollvar", "$basetexturetransform");
        textureScroll.setString("texturescrollrate", "0.6");
        textureScroll.setString("texturescrollangle", "90");

        kv.setString("$envmaptint", "[.7 .7 .7]");
        return kv;
    }

    [[nodiscard]] auto platinum()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "models/player/ct_fbi/ct_fbi_glass");
        kv.setString("$envmap", "env_cubemap");
        kv.setString("$envmaptint", "[.4 .6 .7]");
        return kv;
    }

    [[nodiscard]] auto glass()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "detail/dt_metal1");
        kv.setString("$additive", "1");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$color", "[.05 .05 .05]");
        return kv;
    }

    [[nodiscard]] auto crystal()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "black");
        kv.setString("$bumpmap", "effects/flat_normal");
        kv.setString("$translucent", "1");
        kv.setString("$envmap", "models/effects/crystal_cube_vertigo_hdr");
        kv.setString("$envmapfresnel", "0");
        kv.setString("$phong", "1");
        kv.setString("$phongexponent", "16");
        kv.setString("$phongboost", "2");
        kv.setString("$phongtint", "[.2 .35 .6]");
        return kv;
    }

    [[nodiscard]] auto silver()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "white");
        kv.setString("$bumpmap", "effects/flat_normal");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapfresnel", ".6");
        kv.setString("$phong", "1");
        kv.setString("$phongboost", "2");
        kv.setString("$phongexponent", "8");
        kv.setString("$color2", "[.05 .05 .05]");
        kv.setString("$envmaptint", "[.2 .2 .2]");
        kv.setString("$phongfresnelranges", "[.7 .8 1]");
        kv.setString("$phongtint", "[.8 .9 1]");
        return kv;
    }

    [[nodiscard]] auto gold()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "white");
        kv.setString("$bumpmap", "effects/flat_normal");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapfresnel", ".6");
        kv.setString("$phong", "1");
        kv.setString("$phongboost", "6");
        kv.setString("$phongexponent", "128");
        kv.setString("$phongdisablehalflambert", "1");
        kv.setString("$color2", "[.18 .15 .06]");
        kv.setString("$envmaptint", "[.6 .5 .2]");
        kv.setString("$phongfresnelranges", "[.7 .8 1]");
        kv.setString("$phongtint", "[.6 .5 .2]");
        return kv;
    }

    [[nodiscard]] auto plastic()
    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "black");
        kv.setString("$bumpmap", "models/inventory_items/trophy_majors/matte_metal_normal");
        kv.setString("$additive", "1");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapfresnel", "1");
        kv.setString("$normalmapalphaenvmapmask", "1");
        kv.setString("$phong", "1");
        kv.setString("$phongboost", "20");
        kv.setString("$phongexponent", "3000");
        kv.setString("$phongdisablehalflambert", "1");
        kv.setString("$phongfresnelranges", "[.1 .4 1]");
        kv.setString("$phongtint", "[.8 .9 1]");
        return kv;
    }

private:
    KeyValuesFactory keyValuesFactory;
};
