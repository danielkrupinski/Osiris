#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct EngineSoundPOD;
    struct Vector;
}

class EngineSoundHooks : public RefCountedHook<EngineSoundHooks> {
public:
    explicit EngineSoundHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::EngineSoundPOD* engineSound)
        : hookImpl{ vmtLengthCalculator }, engineSound{ engineSound }
    {
    }

    [[nodiscard]] auto getOriginalEmitSound() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalEmitSound };
    }
    
    static int FASTCALL_CONV emitSound(FASTCALL_THIS(csgo::EngineSoundPOD* thisptr), void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(engineSound));
        originalEmitSound = hookImpl.hook(WIN32_LINUX(5, 6), &emitSound);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(engineSound));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(engineSound));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::EngineSoundPOD* engineSound;

    int (THISCALL_CONV* originalEmitSound)(csgo::EngineSoundPOD* thisptr, void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector* origin, const csgo::Vector* direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams);
};
