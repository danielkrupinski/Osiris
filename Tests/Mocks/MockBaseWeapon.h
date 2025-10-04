#pragma once

#include <gmock/gmock.h>

struct MockBaseWeapon {
    MOCK_METHOD(std::uint64_t(*)(cs2::C_CSWeaponBase*, void*, bool), getSceneObjectUpdater, ());
    MOCK_METHOD(void, setSceneObjectUpdater, (std::uint64_t(*updater)(cs2::C_CSWeaponBase*, void*, bool)));
    MOCK_METHOD(MockBaseEntity&, baseEntity, ());
};
