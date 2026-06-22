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
    using RawType = cs2::CCSPlayerController;

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

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return playerControllerPointer != nullptr;
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
        return offsetToPlayerColor().of(playerControllerPointer).toOptional();
    }

    [[nodiscard]] const char* getName() const noexcept
    {
        // Prefer the sanitized name so it matches the scoreboard and killfeed (e.g. bot names).
        if (const auto name = sanitizedPlayerName(); name && *name)
            return name;
        if (const auto name = playerName(); name && *name)
            return name;
        return nullptr;
    }

    [[nodiscard]] std::optional<bool> isPawnAlive() const noexcept
    {
        if (const auto field = fieldAt<bool>(playerControllerPointer, kOffsetToPawnIsAlive))
            return *field;
        return std::nullopt;
    }

    // A player whose controller is still in the entity system but who has left the
    // server (disconnecting/disconnected) must not be treated as an active player.
    [[nodiscard]] bool isConnected() const noexcept
    {
        if (const auto field = fieldAt<cs2::PlayerConnectedState>(playerControllerPointer, kOffsetToConnectedState))
            return *field == cs2::PlayerConnectedState::Connected;
        return false;
    }

    [[nodiscard]] std::optional<cs2::CEntityHandle> playerPawnHandle() const noexcept
    {
        return entityHandleAt(playerControllerPointer, kOffsetToPlayerPawnHandle);
    }

    [[nodiscard]] std::optional<cs2::CEntityHandle> observerTargetHandle() const noexcept
    {
        const auto observerPawnHandle = entityHandleAt(playerControllerPointer, kOffsetToObserverPawnHandle);
        if (!observerPawnHandle)
            return std::nullopt;

        const auto observerPawn = hookContext.template make<EntitySystem>().getEntityFromHandle(*observerPawnHandle);
        if (!observerPawn)
            return std::nullopt;

        const auto observerServices = fieldAt<void*>(observerPawn, kOffsetToObserverServices);
        if (!observerServices || !*observerServices)
            return std::nullopt;

        return entityHandleAt(*observerServices, kOffsetToObserverTarget);
    }

private:
    template <typename FieldType>
    [[nodiscard]] static const FieldType* fieldAt(const void* object, std::ptrdiff_t offset) noexcept
    {
        if (!object)
            return nullptr;
        return reinterpret_cast<const FieldType*>(static_cast<const std::byte*>(object) + offset);
    }

    [[nodiscard]] static std::optional<cs2::CEntityHandle> entityHandleAt(const void* object, std::ptrdiff_t offset) noexcept
    {
        if (const auto handle = fieldAt<cs2::CEntityHandle>(object, offset); handle && handle->index().isValid())
            return *handle;
        return std::nullopt;
    }

    [[nodiscard]] const char* sanitizedPlayerName() const noexcept
    {
        constexpr auto kOffsetFromPlayerColorToSanitizedName{0x18};
        if (const auto playerColor = offsetToPlayerColor().of(playerControllerPointer).get()) {
            if (const auto name = reinterpret_cast<cs2::CUtlString*>(reinterpret_cast<std::byte*>(playerColor) + kOffsetFromPlayerColorToSanitizedName)->m_pString)
                return name;
        }

        // Fallback from current public schema (CCSPlayerController::m_sSanitizedPlayerName).
        constexpr auto kFallbackOffsetToSanitizedPlayerName{std::int32_t{0x860}};
        if (playerControllerPointer != nullptr) {
            if (const auto name = reinterpret_cast<cs2::CUtlString*>(reinterpret_cast<std::byte*>(playerControllerPointer) + kFallbackOffsetToSanitizedPlayerName)->m_pString)
                return name;
        }
        return nullptr;
    }

    [[nodiscard]] const char* playerName() const noexcept
    {
        // Fallback from current public schema (CBasePlayerController::m_iszPlayerName).
        constexpr auto kFallbackOffsetToPlayerName{std::int32_t{0x6F4}};
        if (playerControllerPointer != nullptr)
            return reinterpret_cast<const char*>(reinterpret_cast<const std::byte*>(playerControllerPointer) + kFallbackOffsetToPlayerName);
        return nullptr;
    }

    [[nodiscard]] auto offsetToPlayerColor() const noexcept
    {
        const auto offset = hookContext.patternSearchResults().template get<OffsetToPlayerColor>();
        if (offset)
            return offset;

        // Fallback for game builds where the color signature is out-of-date.
        constexpr auto kFallbackOffsetToPlayerColor{std::int32_t{0x848}};
        return decltype(offset){kFallbackOffsetToPlayerColor};
    }

    // Current public CS2 schema offsets. Keep these together so post-update validation is mechanical.
    static constexpr std::ptrdiff_t kOffsetToConnectedState{0x6EC};
    static constexpr std::ptrdiff_t kOffsetToPlayerPawnHandle{0x90C};
    static constexpr std::ptrdiff_t kOffsetToObserverPawnHandle{0x910};
    static constexpr std::ptrdiff_t kOffsetToPawnIsAlive{0x914};
    static constexpr std::ptrdiff_t kOffsetToObserverServices{0x11F8};
    static constexpr std::ptrdiff_t kOffsetToObserverTarget{0x4C};

    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
