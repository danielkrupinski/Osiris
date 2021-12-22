#pragma once

#include <cstddef>

#include "Vector.h"

struct AnimState {
    void* This;
    std::byte pad[91];
    void* baseEntity;
    void* activeWeapon;
    void* lastActiveWeapon;
    float lastClientSideAnimationUpdateTime;
    int lastClientSideAnimationUpdateFramecount;
    float eyePitch;
    float eyeYaw;
    float pitch;
    float goalFeetYaw;
    float currentFeetYaw;
    float currentTorsoYaw;
    float unknownVelocityLean;
    float leanAmount;
    std::byte pad1[4];
    float feetCycle;
    float feetYawRate;
    float unknown2;
    float duckAmount;
    float landingDuckAdditiveSomething;
    float unknown3;
    Vector origin;
    Vector lastOrigin;
    float velocityX;
    float velocityY;
    std::byte pad2[4];
    float unknownFloat1;
    std::byte pad3[8];
    float unknownFloat2;
    float unknownFloat3;
    float unknown;
    float speed2D;
    float upVelocity;
    float speedNormalized;
    float footSpeed;
    float footSpeed2;
    float timeSinceStartedMoving;
    float timeSinceStoppedMoving;
    unsigned char onGround;
    unsigned char inHitGroundAnimation;
    std::byte pad4[10];
    float lastOriginZ;
    float headHeightOrOffsetFromHittingGroundAnimation;
    float stopToFullRunningFraction;
    std::byte pad5[532];
    float velocitySubtractY;
};