#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

#include <GameClasses/ClientMode.h>
#include <Hooks/LoopModeGameHook.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Utils/ReturnAddress.h>

class SniperScopeBlurRemover {
public:
    explicit SniperScopeBlurRemover(const ClientPatterns& clientPatterns) noexcept
        : getWorldSessionInClientMode{clientPatterns.getWorldSessionInClientMode()}
        , clientMode{clientPatterns.clientMode()}
    {
    }
        
    void incrementReferenceCount(LoopModeGameHook& loopModeGameHook) noexcept
    {
        assert(referenceCount < (std::numeric_limits<std::size_t>::max)());
        ++referenceCount;
        loopModeGameHook.incrementReferenceCount();
    }

    void decrementReferenceCount(LoopModeGameHook& loopModeGameHook) noexcept
    {
        assert(referenceCount > 0);
        --referenceCount;
        loopModeGameHook.decrementReferenceCount();
    }

    void getWorldSessionHook(ReturnAddress returnAddress) const noexcept
    {
        if (shouldRemoveZoomedSniperEffect(returnAddress))
            clientMode.removeZoomedSniperEffect();
    }

private:
    [[nodiscard]] bool shouldRemoveZoomedSniperEffect(ReturnAddress returnAddress) const noexcept
    {
        return isEnabled() && returnAddress == getWorldSessionInClientMode && clientMode;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return referenceCount > 0;
    }

    ReturnAddress getWorldSessionInClientMode;
    ClientMode clientMode;
    std::size_t referenceCount{0};
};
