#pragma once

#include <functional>

#include <Utils/Pad.h>
#include "Vector.h"
#include "VirtualMethod.h"

namespace csgo
{

struct Entity;

struct BeamInfo {
    int	type;
    Entity* startEnt;
    int startAttachment;
    Entity* endEnt;
    int	endAttachment;
    Vector start;
    Vector end;
    int modelIndex;
    const char* modelName;
    int haloIndex;
    const char* haloName;
    float haloScale;
    float life;
    float width;
    float endWidth;
    float fadeLength;
    float amplitude;
    float brightness;
    float speed;
    int	startFrame;
    float frameRate;
    float red;
    float green;
    float blue;
    bool renderable;
    int segments;
    int	flags;
    Vector ringCenter;
    float ringStartRadius;
    float ringEndRadius;
};

struct Beam {
    PAD(WIN32_LINUX(52, 76));
    int flags;
    PAD(144);
    float die;
};

struct ViewRenderBeamsPOD;

struct ViewRenderBeams : GameClass<ViewRenderBeams, ViewRenderBeamsPOD> {
    VIRTUAL_METHOD(Beam*, createBeamPoints, WIN32_LINUX(12, 9), (BeamInfo& beamInfo), (&beamInfo))
};

}
