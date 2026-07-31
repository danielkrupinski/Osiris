#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Classes/Vector.h>
#include <CS2/Constants/BoneIndex.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/PlayerPawnPatternTypes.h>
#include <Utils/ColorUtils.h>

#include "BaseEntity.h"
#include "C4.h"
#include "HostageServices.h"
#include "WeaponServices.h"

class EntityFromHandleFinder;

template <typename HookContext>
class PlayerController;

template <typename HookContext>
class PlayerPawn {
public:
    using RawType = cs2::C_CSPlayerPawn;

    PlayerPawn(HookContext& hookContext, cs2::C_CSPlayerPawn* playerPawn) noexcept
        : hookContext{hookContext}
        , playerPawn{playerPawn}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(playerPawn);
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return playerPawn != nullptr;
    }


    template <template <typename...> typename EntityType>
    [[nodiscard]] decltype(auto) cast() const noexcept
    {
        if (baseEntity().template is<EntityType>())
            return hookContext.template make<EntityType<HookContext>>(static_cast<typename EntityType<HookContext>::RawType*>(playerPawn));
        return hookContext.template make<EntityType<HookContext>>(nullptr);
    }

    [[nodiscard]] decltype(auto) weaponServices() const noexcept
    {
        return hookContext.template make<WeaponServices>(hookContext.patternSearchResults().template get<OffsetToWeaponServices>().of(playerPawn).valueOr(nullptr));
    }

    [[nodiscard]] decltype(auto) weapons() const noexcept
    {
        return weaponServices().weapons();
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return baseEntity().teamNumber();
    }

    [[nodiscard]] std::optional<bool> isAlive() const noexcept
    {
        return baseEntity().isAlive();
    }

    [[nodiscard]] decltype(auto) playerController() const noexcept
    {
        const auto playerControllerHandle = hookContext.patternSearchResults().template get<OffsetToPlayerController>().of(playerPawn).get();
        if (!playerControllerHandle)
            return hookContext.template make<PlayerController>(nullptr);
        return hookContext.template make<PlayerController>(static_cast<cs2::CCSPlayerController*>(hookContext.template make<EntitySystem>().getEntityFromHandle(*playerControllerHandle)));
    }

    [[nodiscard]] auto health() const noexcept
    {
        return baseEntity().health();
    }

    [[nodiscard]] auto hasImmunity() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPlayerPawnImmunity>().of(playerPawn).toOptional();
    }

    [[nodiscard]] decltype(auto) absOrigin() const noexcept
    {
        return baseEntity().absOrigin();
    }

    [[nodiscard]] Optional<cs2::Vector> bonePosition(cs2::BoneIndex boneIndex) const noexcept
    {
        if (!playerPawn)
            return {};

        const auto gameSceneNode = gameSceneNodePointer();
        if (!gameSceneNode)
            return {};

        const auto boneArray = *reinterpret_cast<std::byte**>(
            gameSceneNode + kOffsetToModelState + kOffsetToBoneArray);
        if (!boneArray)
            return {};

        const auto boneData = boneArray + static_cast<std::int32_t>(boneIndex) * kBoneDataStride;
        const auto modelPos = *reinterpret_cast<const cs2::Vector*>(boneData);

        // Transform model-space bone position to world-space using m_nodeToWorld
        // Source 2 matrix3x4_t = 4 column vectors each 3 floats: [col0][col1][col2][col3]
        // world = col0 * x + col1 * y + col2 * z + col3 (translation)
        const auto m = reinterpret_cast<const float*>(gameSceneNode + kNodeToWorldOffset);
        return cs2::Vector{
            m[0]*modelPos.x + m[3]*modelPos.y + m[6]*modelPos.z  + m[9],
            m[1]*modelPos.x + m[4]*modelPos.y + m[7]*modelPos.z  + m[10],
            m[2]*modelPos.x + m[5]*modelPos.y + m[8]*modelPos.z + m[11]
        };
    }

    [[nodiscard]] Optional<cs2::Vector> headPosition() const noexcept
    {
        return bonePosition(cs2::BoneIndex::Head);
    }

    [[nodiscard]] Optional<cs2::Vector> chestPosition() const noexcept
    {
        return bonePosition(cs2::BoneIndex::Chest);
    }

    [[nodiscard]] Optional<cs2::Vector> stomachPosition() const noexcept
    {
        return bonePosition(cs2::BoneIndex::Stomach);
    }

    [[nodiscard]] bool isDormant() const noexcept
    {
        if (!playerPawn)
            return true;
        const auto sceneNode = gameSceneNodePointer();
        if (!sceneNode)
            return true;
        return *reinterpret_cast<bool*>(sceneNode + kDormantOffset);
    }

    // m_vecAbsVelocity on C_BaseEntity. World-space velocity in units/sec.
    [[nodiscard]] cs2::Vector absVelocity() const noexcept
    {
        if (!playerPawn)
            return {};
        return *reinterpret_cast<const cs2::Vector*>(
            reinterpret_cast<const std::byte*>(playerPawn) + kOffsetToAbsVelocity);
    }

    // m_vecViewOffset on C_BaseModelEntity. Offset from absOrigin to the eye
    // position. Game updates this every frame including crouch/jump animations,
    // so absOrigin + viewOffset is the *real* eye location even when the
    // player is ducked or mid-animation. Schema-verified by cs2-dumper.
    [[nodiscard]] cs2::Vector viewOffset() const noexcept
    {
        if (!playerPawn)
            return {};
        return *reinterpret_cast<const cs2::Vector*>(
            reinterpret_cast<const std::byte*>(playerPawn) + kOffsetToViewOffset);
    }

    [[nodiscard]] cs2::C_CSPlayerPawn* rawPointer() const noexcept
    {
        return playerPawn;
    }

    [[nodiscard]] bool isControlledByLocalPlayer() const noexcept
    {
        return playerController() == hookContext.localPlayerController();
    }

    [[nodiscard]] std::optional<bool> isEnemy() const noexcept
    {
        return teamNumber() != hookContext.localPlayerController().teamNumber() || teammatesAreEnemies();
    }

    [[nodiscard]] bool isTTorCT() const noexcept
    {
        const auto _teamNumber = teamNumber();
        return _teamNumber == TeamNumber::TT || _teamNumber == TeamNumber::CT;
    }

    [[nodiscard]] auto isPickingUpHostage() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToIsPickingUpHostage>().of(playerPawn).toOptional();
    }

    [[nodiscard]] auto isDefusing() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToIsDefusing>().of(playerPawn).toOptional();
    }

    [[nodiscard]] auto isScoped() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToIsScoped>().of(playerPawn).toOptional();
    }

    [[nodiscard]] bool isRescuingHostage() const noexcept
    {
        return hostageServices().hasCarriedHostage();
    }

    [[nodiscard]] bool isCarryingC4() const noexcept
    {
        return weapons().template has<C4>();
    }

    [[nodiscard]] decltype(auto) carriedC4() const noexcept
    {
        return weapons().template get<C4>();
    }

    [[nodiscard]] float getRemainingFlashBangTime() const noexcept
    {
        const auto curTime = hookContext.globalVars().curtime();
        if (!curTime.hasValue())
            return 0.0f;
        const auto flashBangEndTime = hookContext.patternSearchResults().template get<OffsetToFlashBangEndTime>().of(playerPawn).get();
        if (!flashBangEndTime)
            return 0.0f;
        if (*flashBangEndTime <= curTime.value())
            return 0.0f;
        return *flashBangEndTime - curTime.value();
    }

    [[nodiscard]] decltype(auto) getActiveWeapon() const noexcept
    {
        return weaponServices().getActiveWeapon();
    }

    [[nodiscard]] auto getSceneObjectUpdater() const noexcept
    {
        return reinterpret_cast<std::uint64_t(*)(cs2::C_CSPlayerPawn*, void*, bool)>(sceneObjectUpdaterHandle() ? sceneObjectUpdaterHandle()->updaterFunction : nullptr);
    }

    void setSceneObjectUpdater(auto x) const noexcept
    {
        if (sceneObjectUpdaterHandle())
            sceneObjectUpdaterHandle()->updaterFunction = reinterpret_cast<std::uint64_t(*)(void*, void*, bool)>(x);
    }

    [[nodiscard]] decltype(auto) isUsingSniperRifle() const
    {
        return getActiveWeapon().isSniperRifle();
    }

private:
    [[nodiscard]] std::byte* gameSceneNodePointer() const noexcept
    {
        return reinterpret_cast<std::byte*>(
            hookContext.patternSearchResults().template get<OffsetToGameSceneNode>()
                .of(playerPawn).valueOr(nullptr));
    }

    [[nodiscard]] auto sceneObjectUpdaterHandle() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPlayerPawnSceneObjectUpdaterHandle>().of(playerPawn).valueOr(nullptr);
    }

    [[nodiscard]] decltype(auto) hostageServices() const noexcept
    {
        return hookContext.template make<HostageServices>(hookContext.patternSearchResults().template get<OffsetToHostageServices>().of(playerPawn).valueOr(nullptr));
    }

    [[nodiscard]] bool teammatesAreEnemies() const noexcept
    {
        return hookContext.cvarSystem().template getConVarValue<cs2::mp_teammates_are_enemies>().value_or(true);
    }

    // Hardcoded offsets for bone position reading (from cs2-dumper schema).
    static constexpr std::ptrdiff_t kOffsetToModelState{0x140};
    static constexpr std::ptrdiff_t kOffsetToBoneArray{0x80};
    // C_BaseEntity::m_vecAbsVelocity (cs2-dumper, build 14171).
    static constexpr std::ptrdiff_t kOffsetToAbsVelocity{0x3F8};
    // C_BaseModelEntity::m_vecViewOffset (cs2-dumper, build 14171).
    static constexpr std::ptrdiff_t kOffsetToViewOffset{0xE78};
    static constexpr std::ptrdiff_t kNodeToWorldOffset{0x10};
    static constexpr std::ptrdiff_t kDormantOffset{0x103};
    static constexpr std::ptrdiff_t kBoneDataStride{32};

    HookContext& hookContext;
    cs2::C_CSPlayerPawn* playerPawn;
};
