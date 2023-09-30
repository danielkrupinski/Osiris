#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtSwapper.h>

class LoopModeGameHook : public RefCountedHook<LoopModeGameHook> {
public:
    explicit LoopModeGameHook(const VmtLengthCalculator& vmtLengthCalculator) noexcept
        : hook{ vmtLengthCalculator }
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
        return loopModeGame && *loopModeGame && hook.isInstalled(*reinterpret_cast<std::uintptr_t**>(*loopModeGame));
    }

    void install() noexcept
    {
        if (loopModeGame && *loopModeGame && hook.install(*reinterpret_cast<std::uintptr_t**>(*loopModeGame))) {
            originalGetWorldSession = hook.hook(0, &getWorldSession);
        }
    }

    friend class RefCountedHook;

    cs2::CLoopModeGame** loopModeGame{ ClientPatterns::loopModeGame() };
    VmtSwapper hook;
    cs2::CLoopModeGame::getWorldSession originalGetWorldSession{ nullptr };
};
