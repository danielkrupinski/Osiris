#include <stdexcept>
#include <Windows.h>

#include "Interfaces.h"
#include "Memory.h"

Memory::Memory() noexcept
{
    try {
        present = findPattern<>("gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB", 2);
        reset = findPattern<>("gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8", 9);
        clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[10] + 5);
        input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces.client))[16] + 1);
        globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[11] + 10);
        glowObjectManager = *findPattern<GlowObjectManager**>("client_panorama", "\x0F\x11\x05????\x83\xC8\x01", 3);
        disablePostProcessing = *findPattern<bool**>("client_panorama", "\x83\xEC\x4C\x80\x3D", 5);
        loadSky = findPattern<decltype(loadSky)>("engine", "\x55\x8B\xEC\x81\xEC????\x56\x57\x8B\xF9\xC7\x45");
        setClanTag = findPattern<decltype(setClanTag)>("engine", "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15");
        lineGoesThroughSmoke = findPattern<decltype(lineGoesThroughSmoke)>("client_panorama", "\x55\x8B\xEC\x83\xEC\x08\x8B\x15????\x0F\x57\xC0");
        smokeCount = *(reinterpret_cast<int**>(lineGoesThroughSmoke) + 2);
        cameraThink = findPattern<>("client_panorama", "\x85\xC0\x75\x30\x38\x86");
        revealRanks = findPattern<decltype(revealRanks)>("client_panorama", "\x55\x8B\xEC\x8B\x0D????\x85\xC9\x75\x28\xA1????\x68????\x8B\x08\x8B\x01\xFF\x50\x04\x85\xC0\x74\x0B\x8B\xC8\xE8????\x8B\xC8\xEB\x02\x33\xC9\x89\x0D????\x8B\x45\x08");
        acceptMatch = findPattern<decltype(acceptMatch)>("client_panorama", "\x55\x8B\xEC\x83\xE4\xF8\x8B\x4D\x08\xBA????\xE8????\x85\xC0\x75\x12");
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}
