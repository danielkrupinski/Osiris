#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtSwapper.h>

class LoopModeGameHook : public RefCountedHook<LoopModeGameHook> {
public:
    LoopModeGameHook(const ClientPatterns& clientPatterns, const VmtLengthCalculator& vmtLengthCalculator) noexcept
        : loopModeGame{clientPatterns.loopModeGame()}
        , vmtLengthCalculator{vmtLengthCalculator}
    {
    }

private:
    static void* getWorldSession(cs2::CLoopModeGame* thisptr) noexcept;

    void uninstall() const noexcept
    {
        if (loopModeGame && *loopModeGame)
            hook.uninstall(*reinterpret_cast<std::uintptr_t**>(*loopModeGame));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hook.wasEverInstalled() && loopModeGame && *loopModeGame && hook.isInstalled(*reinterpret_cast<std::uintptr_t**>(*loopModeGame));
    }

    void install() noexcept
    {
        if (loopModeGame && *loopModeGame && hook.install(vmtLengthCalculator, *reinterpret_cast<std::uintptr_t**>(*loopModeGame))) {
            originalGetWorldSession = hook.hook(0, &getWorldSession);
        }
    }

    friend class RefCountedHook;

    cs2::CLoopModeGame** loopModeGame;
    VmtLengthCalculator vmtLengthCalculator;
    VmtSwapper hook;
    cs2::CLoopModeGame::getWorldSession originalGetWorldSession{ nullptr };
};
