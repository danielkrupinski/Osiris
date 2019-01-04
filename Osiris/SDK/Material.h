#pragma once

#include "Utils.h"

enum class MaterialVar {
    DEBUG = (1 << 0),
    NO_DEBUG_OVERRIDE = (1 << 1),
    NO_DRAW = (1 << 2),
    USE_IN_FILLRATE_MODE = (1 << 3),
    VERTEXCOLOR = (1 << 4),
    VERTEXALPHA = (1 << 5),
    SELFILLUM = (1 << 6),
    ADDITIVE = (1 << 7),
    ALPHATEST = (1 << 8),
    ZNEARER = (1 << 10),
    MODEL = (1 << 11),
    FLAT = (1 << 12),
    NOCULL = (1 << 13),
    NOFOG = (1 << 14),
    IGNOREZ = (1 << 15),
    DECAL = (1 << 16),
    ENVMAPSPHERE = (1 << 17),
    ENVMAPCAMERASPACE = (1 << 19),
    BASEALPHAENVMAPMASK = (1 << 20),
    TRANSLUCENT = (1 << 21),
    NORMALMAPALPHAENVMAPMASK = (1 << 22),
    NEEDS_SOFTWARE_SKINNING = (1 << 23),
    OPAQUETEXTURE = (1 << 24),
    ENVMAPMODE = (1 << 25),
    SUPPRESS_DECALS = (1 << 26),
    HALFLAMBERT = (1 << 27),
    WIREFRAME = (1 << 28),
    ALLOWALPHATOCOVERAGE = (1 << 29),
    ALPHA_MODIFIED_BY_PROXY = (1 << 30),
    VERTEXFOG = (1 << 31),
};

class Material {
public:
    void incrementReferenceCount()
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 12)(this);
    }

    void alphaModulate(float alpha)
    {
        callVirtualFunction<void(__thiscall*)(void*, float)>(this, 27)(this, alpha);
    }

    void colorModulate(float r, float g, float b)
    {
        callVirtualFunction<void(__thiscall*)(void*, float, float, float)>(this, 28)(this, r, g, b);
    }

    void setMaterialVarFlag(MaterialVar flag, bool isOn)
    {
        callVirtualFunction<void(__thiscall*)(void*, MaterialVar, bool)>(this, 29)(this, flag, isOn);
    }

    void setShader(const char* shaderName)
    {
        callVirtualFunction<void(__thiscall*)(void*, const char*)>(this, 34)(this, shaderName);
    }
};
