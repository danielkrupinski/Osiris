#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
#include "PlayerPawn.h"

template <typename HookContext>
class PlayerController {
public:
    PlayerController(HookContext& hookContext, cs2::CCSPlayerController* playerControllerPointer) noexcept
        : hookContext{hookContext}
        , playerControllerPointer{playerControllerPointer}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(playerControllerPointer);
    }

    [[nodiscard]] bool operator==(const PlayerController& other) const noexcept
    {
        return playerControllerPointer != nullptr && playerControllerPointer == other.playerControllerPointer;
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return baseEntity().teamNumber();
    }

    [[nodiscard]] decltype(auto) pawn() const noexcept
    {
        const auto playerPawnHandle = hookContext.patternSearchResults().template get<OffsetToBasePawnHandle>().of(playerControllerPointer).get();
        if (!playerPawnHandle)
            return hookContext.template make<BaseEntity>(nullptr);
        return hookContext.template make<EntitySystem>().getEntityFromHandle2(*playerPawnHandle);
    }

    [[nodiscard]] decltype(auto) playerColorIndex() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPlayerColor>().of(playerControllerPointer).toOptional();
    }

    [[nodiscard]] const char* getName() const noexcept
    {
        // Prefer the sanitized name so it matches the scoreboard and the killfeed (e.g. bot names).
        if (const auto name = sanitizedPlayerName(); name && *name)
            return name;
        if (const auto name = playerName(); name && *name)
            return name;
        return nullptr;
    }

private:
    [[nodiscard]] const char* sanitizedPlayerName() const noexcept
    {
        if (playerControllerPointer == nullptr)
            return nullptr;

        // m_sSanitizedPlayerName is located after m_iCompTeammateColor in CCSPlayerController,
        // so it can be reached through the pattern-resolved player color field.
        if (const auto playerColor = hookContext.patternSearchResults().template get<OffsetToPlayerColor>().of(playerControllerPointer).get()) {
            constexpr auto kOffsetFromPlayerColorToSanitizedPlayerName{0x20};
            if (const auto name = reinterpret_cast<cs2::CCSPlayerController::m_sSanitizedPlayerName*>(reinterpret_cast<std::byte*>(playerColor) + kOffsetFromPlayerColorToSanitizedPlayerName)->m_pString)
                return name;
        }

        // Fallback offsets taken from the public schema of the tested game builds,
        // used when the player color pattern does not match.
        constexpr auto kFallbackOffsetToSanitizedPlayerName{std::int32_t{0x878}};
        if (const auto name = reinterpret_cast<cs2::CCSPlayerController::m_sSanitizedPlayerName*>(reinterpret_cast<std::byte*>(playerControllerPointer) + kFallbackOffsetToSanitizedPlayerName)->m_pString)
            return name;
        return nullptr;
    }

    [[nodiscard]] const char* playerName() const noexcept
    {
        // CBasePlayerController::m_iszPlayerName, used when the sanitized name is empty.
        constexpr auto kFallbackOffsetToPlayerName{std::int32_t{0x6FC}};
        if (playerControllerPointer == nullptr)
            return nullptr;
        return reinterpret_cast<const char*>(reinterpret_cast<const std::byte*>(playerControllerPointer) + kFallbackOffsetToPlayerName);
    }

    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
