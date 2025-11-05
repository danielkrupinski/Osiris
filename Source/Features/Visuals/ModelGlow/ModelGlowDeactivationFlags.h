#pragma once

enum class ModelGlowDeactivationFlags {
    ModelGlowDeactivating = 1 << 0,
    PlayerModelGlowDeactivating = 1 << 1,
    WeaponModelGlowDeactivating = 1 << 2,
    DroppedBombModelGlowDeactivating = 1 << 3,
    TickingBombModelGlowDeactivating = 1 << 4,
    DefuseKitModelGlowDeactivating = 1 << 5,
    GrenadeProjectileModelGlowDeactivating = 1 << 6
};
