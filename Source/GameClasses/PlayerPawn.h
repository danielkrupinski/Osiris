#pragma once

#include <optional>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Constants/EntityHandle.h>
#include <FeatureHelpers/LifeState.h>
#include <FeatureHelpers/TeamNumber.h>
#include <Utils/ColorUtils.h>

#include "BaseEntity.h"
#include "C4.h"
#include "GameSceneNode.h"
#include "HostageServices.h"
#include "PlayerWeapons.h"
#include "WeaponServices.h"

class EntityFromHandleFinder;

template <typename HookContext>
class PlayerController;

template <typename HookContext>
class PlayerPawn {
public:
    PlayerPawn(HookContext& hookContext, cs2::C_CSPlayerPawn* playerPawn) noexcept
        : hookContext{hookContext}
        , playerPawn{playerPawn}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(playerPawn);
    }

    [[nodiscard]] decltype(auto) weaponServices() const noexcept
    {
        return hookContext.template make<WeaponServices>(hookContext.clientPatternSearchResults().template get<OffsetToWeaponServices>().of(playerPawn).valueOr(nullptr));
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
        const auto playerControllerHandle = hookContext.clientPatternSearchResults().template get<OffsetToPlayerController>().of(playerPawn).get();
        if (!playerControllerHandle)
            return hookContext.template make<PlayerController>(nullptr);
        return hookContext.template make<PlayerController>(static_cast<cs2::CCSPlayerController*>(hookContext.template make<EntitySystem>().getEntityFromHandle(*playerControllerHandle)));
    }

    [[nodiscard]] auto health() const noexcept
    {
        return baseEntity().health();
    }

    [[nodiscard]] std::optional<cs2::Color> healthColor() const noexcept
    {
        if (const auto healthValue = health(); healthValue.hasValue())
            return getColorOfHealthFraction(std::clamp(healthValue.value(), 0, 100) / 100.0f);
        return {};
    }

    [[nodiscard]] auto hasImmunity() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToPlayerPawnImmunity>().of(playerPawn).toOptional();
    }

    [[nodiscard]] decltype(auto) absOrigin() const noexcept
    {
        return baseEntity().gameSceneNode().absOrigin();
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
        return hookContext.clientPatternSearchResults().template get<OffsetToIsPickingUpHostage>().of(playerPawn).toOptional();
    }

    [[nodiscard]] auto isDefusing() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToIsDefusing>().of(playerPawn).toOptional();
    }

    [[nodiscard]] bool isRescuingHostage() const noexcept
    {
        return hostageServices().hasCarriedHostage();
    }

    [[nodiscard]] bool isCarryingC4() const noexcept
    {
        return weapons().template has<C4<HookContext>>();
    }

    [[nodiscard]] float getRemainingFlashBangTime() const noexcept
    {
        const auto curTime = hookContext.globalVars().curtime();
        if (!curTime.hasValue())
            return 0.0f;
        const auto flashBangEndTime = hookContext.clientPatternSearchResults().template get<OffsetToFlashBangEndTime>().of(playerPawn).get();
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

private:
    [[nodiscard]] decltype(auto) hostageServices() const noexcept
    {
        return hookContext.template make<HostageServices>(hookContext.clientPatternSearchResults().template get<OffsetToHostageServices>().of(playerPawn).valueOr(nullptr));
    }

    [[nodiscard]] static cs2::Color getColorOfHealthFraction(float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction, 0.7f, 1.0f);
    }

    [[nodiscard]] bool teammatesAreEnemies() const noexcept
    {
        auto conVarAccessor = hookContext.getConVarAccessor();
        if (!conVarAccessor.template requestConVar<cs2::mp_teammates_are_enemies>())
            return true;
        return conVarAccessor.template getConVarValue<cs2::mp_teammates_are_enemies>();
    }

    HookContext& hookContext;
    cs2::C_CSPlayerPawn* playerPawn;
};
