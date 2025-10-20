#pragma once

#include <optional>
#include <GameClient/Entities/TeamNumber.h>
#include <Utils/Optional.h>

struct MockBaseEntity;
struct MockBaseWeapon;
struct MockPlayerController;

struct MockPlayerPawn {
    MOCK_METHOD(MockBaseEntity&, baseEntity, ());
    MOCK_METHOD(std::optional<bool>, isAlive, ());
    MOCK_METHOD(Optional<int>, health, ());
    MOCK_METHOD(bool, isControlledByLocalPlayer, ());
    MOCK_METHOD(bool, isTTorCT, ());
    MOCK_METHOD(std::optional<bool>, isEnemy, ());
    MOCK_METHOD(TeamNumber, teamNumber, ());
    MOCK_METHOD(Optional<bool>, hasImmunity, ());
    MOCK_METHOD(MockPlayerController&, playerController, ());
    MOCK_METHOD(std::uint64_t(*)(cs2::C_CSPlayerPawn*, void*, bool), getSceneObjectUpdater, ());
    MOCK_METHOD(void, setSceneObjectUpdater, (std::uint64_t(*updater)(cs2::C_CSPlayerPawn*, void*, bool)));
    MOCK_METHOD(bool, operatorBool, (), (const));
    MOCK_METHOD(Optional<bool>, isScoped, ());
    MOCK_METHOD(MockBaseWeapon&, getActiveWeapon, ());
    MOCK_METHOD(bool, isUsingSniperRifle, ());

    [[nodiscard]] explicit operator bool() const
    {
        return operatorBool();
    }
};
