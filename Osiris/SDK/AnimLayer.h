#pragma once

struct AnimLayer {
    bool clientBlend;
    float blendIn;
    void* studioHdr;
    int dispatchedSrc;
    int dispatchedDst;
    unsigned int order;
    unsigned int sequence;
    float prevCycle;
    float weight;
    float weightDeltaRate;
    float playbackRate;
    float cycle;
    void* owner;
    int invalidatePhysicsBits;
};