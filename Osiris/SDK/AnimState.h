#pragma once

#include <cstddef>
#include "Vector.h"


struct AnimState {
   void* This;
    std::byte pad[91];
    void* BaseEntity;
    void* ActiveWeapon;
    void* LastActiveWeapon;
    float LastClientSideAnimationUpdateTime;
    int LastClientSideAnimationUpdateFramecount;
    float EyePitch;
    float EyeYaw;
    float Pitch;
    float GoalFeetYaw;
    float CurrentFeetYaw;
    float CurrentTorsoYaw;
    float UnknownVelocityLean;
    float LeanAmount;
    std::byte pad1[4];
    float FeetCycle;
    float FeetYawRate;
    float Unknown2;
    float duckAmount;
     float LandingDuckAdditiveSomething;
    float Unknown3;
    Vector Origin;
    Vector LastOrigin;
    float VelocityX;
    float VelocityY;
    std::byte pad2[4];
    float UnknownFloat1;
    std::byte pad3[8];
    float UnknownFloat2;
    float UnknownFloat3;
    float unknown;
    float speed_2d;
    float UpVelocity;
    float SpeedNormalized;
    float footSpeed;
    float footSpeed2;
   float TimeSinceStartedMoving;
    float TimeSinceStoppedMoving;
    unsigned char OnGround;
    unsigned char InHitGroundAnimation;
    std::byte pad4[10];
    float LastOriginZ;
    float HeadHeightOrOffsetFromHittingGroundAnimation;
    float stopToFullRunningFraction;
    std::byte pad5[4];
    float UnknownFraction;
    std::byte pad6[526];
    float velocitySubtractY;
};
