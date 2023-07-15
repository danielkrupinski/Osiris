#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct EngineSoundPOD;
    struct Vector;
}

class EngineSoundHooks {
public:
    explicit EngineSoundHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::EngineSoundPOD* engineSound)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(engineSound));
        originalEmitSound = reinterpret_cast<decltype(originalEmitSound)>(hookImpl.hook(WIN32_LINUX(5, 6), std::uintptr_t(&emitSound)));
    }

    void uninstall(csgo::EngineSoundPOD* engineSound)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(engineSound));
    }

    [[nodiscard]] auto getOriginalEmitSound() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalEmitSound };
    }
    
    static int FASTCALL_CONV emitSound(FASTCALL_THIS(csgo::EngineSoundPOD* thisptr), void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept;

private:
    HookType hookImpl;

    int (THISCALL_CONV* originalEmitSound)(csgo::EngineSoundPOD* thisptr, void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector* origin, const csgo::Vector* direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams);
};
