#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/ConVarTypes.h>
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

    template <template <typename...> typename EntityType>
    [[nodiscard]] decltype(auto) cast() const noexcept
    {
        if (baseEntity().template is<EntityType>())
            return hookContext.template make<EntityType<HookContext>>(static_cast<typename EntityType<HookContext>::RawType*>(playerPawn));
        return hookContext.template make<EntityType<HookContext>>(nullptr);
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

    [[nodiscard]] std::optional<cs2::Color> healthColor(float saturation = 0.7f) const noexcept
    {
        if (const auto healthValue = health(); healthValue.hasValue())
            return getColorOfHealthFraction(saturation, std::clamp(healthValue.value(), 0, 100) / 100.0f);
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

    [[nodiscard]] auto getSceneObjectUpdater() const noexcept
    {
        return reinterpret_cast<std::uint64_t(*)(cs2::C_CSPlayerPawn*, void*, bool)>(sceneObjectUpdaterHandle() ? sceneObjectUpdaterHandle()->updaterFunction : nullptr);
    }

    void setSceneObjectUpdater(auto x) const noexcept
    {
        if (sceneObjectUpdaterHandle())
            sceneObjectUpdaterHandle()->updaterFunction = reinterpret_cast<std::uint64_t(*)(void*, void*, bool)>(x);
    }

private:
    [[nodiscard]] auto sceneObjectUpdaterHandle() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToPlayerPawnSceneObjectUpdaterHandle>().of(playerPawn).valueOr(nullptr);
    }

    [[nodiscard]] decltype(auto) hostageServices() const noexcept
    {
        return hookContext.template make<HostageServices>(hookContext.clientPatternSearchResults().template get<OffsetToHostageServices>().of(playerPawn).valueOr(nullptr));
    }

    [[nodiscard]] static cs2::Color getColorOfHealthFraction(float saturation, float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction, saturation, 1.0f);
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
