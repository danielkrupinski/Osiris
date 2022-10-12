#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <span>
#include <string_view>
#include <utility>

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#elif __linux__
#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "Interfaces.h"
#include "Memory.h"
#include "SDK/LocalPlayer.h"

#include "SafeAddress.h"

static std::span<const std::byte> getModuleInformation(const char* name) noexcept
{
#ifdef _WIN32
    if (HMODULE handle = GetModuleHandleA(name)) {
        if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo)))
            return { reinterpret_cast<const std::byte*>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage };
    }
    return {};
#elif __linux__
    struct ModuleInfo_ {
        const char* name;
        void* base = nullptr;
        std::size_t size = 0;
    } moduleInfo;

    moduleInfo.name = name;

    const linux_platform::SharedObject so{ linux_platform::DynamicLibraryWrapper{}, name };

    const auto linkMap = so.getView().getLinkMap();
    if (linkMap) {
        if (const auto fd = open(linkMap->l_name, O_RDONLY); fd >= 0) {
            if (struct stat st; fstat(fd, &st) == 0) {
                if (const auto map = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0); map != MAP_FAILED) {
                    const auto ehdr = (ElfW(Ehdr)*)map;
                    const auto shdrs = (ElfW(Shdr)*)(std::uintptr_t(ehdr) + ehdr->e_shoff);
                    const auto strTab = (const char*)(std::uintptr_t(ehdr) + shdrs[ehdr->e_shstrndx].sh_offset);

                    for (auto i = 0; i < ehdr->e_shnum; ++i) {
                        const auto shdr = (ElfW(Shdr)*)(std::uintptr_t(shdrs) + i * ehdr->e_shentsize);

                        if (std::strcmp(strTab + shdr->sh_name, ".text") != 0)
                            continue;

                        moduleInfo.base = (void*)(linkMap->l_addr + shdr->sh_offset);
                        moduleInfo.size = shdr->sh_size;
                        munmap(map, st.st_size);
                        close(fd);
                        break;
                    }
                    munmap(map, st.st_size);
                }
            }
            close(fd);
        }
    }
    return { reinterpret_cast<const std::byte*>(moduleInfo.base), moduleInfo.size };
#endif
}

[[nodiscard]] static auto generateBadCharTable(std::string_view pattern) noexcept
{
    assert(!pattern.empty());

    std::array<std::size_t, (std::numeric_limits<std::uint8_t>::max)() + 1> table;

    auto lastWildcard = pattern.rfind('?');
    if (lastWildcard == std::string_view::npos)
        lastWildcard = 0;

    const auto defaultShift = (std::max)(std::size_t(1), pattern.length() - 1 - lastWildcard);
    table.fill(defaultShift);

    for (auto i = lastWildcard; i < pattern.length() - 1; ++i)
        table[static_cast<std::uint8_t>(pattern[i])] = pattern.length() - 1 - i;

    return table;
}

template <bool ReportNotFound = true>
static std::uintptr_t findPattern(std::span<const std::byte> bytes, std::string_view pattern) noexcept
{
    static auto id = 0;
    ++id;

    if (!bytes.empty()) {
        const auto lastIdx = pattern.length() - 1;
        const auto badCharTable = generateBadCharTable(pattern);

        auto start = reinterpret_cast<const char*>(bytes.data());
        const auto end = start + bytes.size() - pattern.length();

        while (start <= end) {
            int i = lastIdx;
            while (i >= 0 && (pattern[i] == '?' || start[i] == pattern[i]))
                --i;

            if (i < 0)
                return reinterpret_cast<std::uintptr_t>(start);

            start += badCharTable[static_cast<std::uint8_t>(start[lastIdx])];
        }
    }

    assert(false);
#ifdef _WIN32
    if constexpr (ReportNotFound)
        MessageBoxA(nullptr, ("Failed to find pattern #" + std::to_string(id) + '!').c_str(), "Osiris", MB_OK | MB_ICONWARNING);
#endif
    return 0;
}

template <bool ReportNotFound>
std::uintptr_t findPattern(const char* moduleName, std::string_view pattern) noexcept
{
    return findPattern<ReportNotFound>(getModuleInformation(moduleName), pattern);
}

Memory::Memory(std::uintptr_t clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept
#ifdef _WIN32
    : viewRenderBeams{ retSpoofGadgets.jmpEbxInClient, *reinterpret_cast<std::uintptr_t*>(findPattern(CLIENT_DLL, "\xB9????\x0F\x11\x44\x24?\xC7\x44\x24?????\xF3\x0F\x10\x84\x24") + 1) },
      weaponSystem{ retSpoofGadgets.jmpEbxInClient, SafeAddress{ findPattern(CLIENT_DLL, "\x8B\x35????\xFF\x10\x0F\xB7\xC0") }.add(2).deref().get() }
#else
    : viewRenderBeams{ retSpoofGadgets.jmpEbxInClient, SafeAddress{ findPattern(CLIENT_DLL, "\x4C\x89\xF6\x4C\x8B\x25????\x48\x8D\x05") }.add(6).relativeToAbsolute().deref<2>().get() },
      weaponSystem{ retSpoofGadgets.jmpEbxInClient, SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8B\x58\x10\x48\x8B\x07\xFF\x10") }.add(12).relativeToAbsolute().deref().get() }
#endif
{
#ifdef _WIN32
    present = SafeAddress{ findPattern("gameoverlayrenderer", "\xFF\x15????\x8B\xF0\x85\xFF") }.add(2).get();
    reset = SafeAddress{ findPattern("gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xD8") }.add(9).get();

    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(clientInterface))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(clientInterface))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(clientInterface))[11] + 10);
    glowObjectManager = reinterpret_cast<GlowObjectManager*>(SafeAddress{ findPattern(CLIENT_DLL, "\x0F\x11\x05????\x83\xC8\x01") }.add(3).deref().get());
    disablePostProcessing = reinterpret_cast<bool*>(SafeAddress{ findPattern(CLIENT_DLL, "\x83\xEC\x4C\x80\x3D") }.add(5).deref().get());
    loadSky = reinterpret_cast<decltype(loadSky)>(SafeAddress{ findPattern(ENGINE_DLL, "\xE8????\x84\xC0\x74\x2D\xA1") }.add(1).relativeToAbsolute().get());
    setClanTag = reinterpret_cast<decltype(setClanTag)>(findPattern(ENGINE_DLL, "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15"));
    lineGoesThroughSmoke = reinterpret_cast<decltype(lineGoesThroughSmoke)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x4C\x24\x30\x33\xD2") }.add(1).relativeToAbsolute().get());
    cameraThink = findPattern(CLIENT_DLL, "\x85\xC0\x75\x30\x38\x86");
    getSequenceActivity = reinterpret_cast<decltype(getSequenceActivity)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83"));
    isOtherEnemy = reinterpret_cast<decltype(isOtherEnemy)>(SafeAddress{ findPattern(CLIENT_DLL, "\x8B\xCE\xE8????\x02\xC0") }.add(3).relativeToAbsolute().get());
    auto temp = SafeAddress{ findPattern(CLIENT_DLL, "\xB9????\xE8????\x8B\x5D\x08") }.add(1);
    hud = SafeAddress{ temp }.deref().get();
    findHudElement = reinterpret_cast<decltype(findHudElement)>(temp.add(5).relativeToAbsolute().get());
    clearHudWeapon = reinterpret_cast<decltype(clearHudWeapon)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\xF0\xC6\x44\x24??\xC6\x44\x24") }.add(1).relativeToAbsolute().get());
    itemSystem = reinterpret_cast<decltype(itemSystem)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x0F") }.add(1).relativeToAbsolute().get());
    setAbsOrigin = reinterpret_cast<decltype(setAbsOrigin)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\xEB\x19\x8B\x07") }.add(1).relativeToAbsolute().get());
    insertIntoTree = SafeAddress{ findPattern(CLIENT_DLL, "\x56\x52\xFF\x50\x18") }.add(5).get();
    dispatchSound = reinterpret_cast<int*>(SafeAddress{ findPattern(ENGINE_DLL, "\x74\x0B\xE8????\x8B\x3D") }.add(3).get());
    traceToExit = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x4C\xF3\x0F\x10\x75");
    viewRender = reinterpret_cast<ViewRender*>(SafeAddress{ findPattern(CLIENT_DLL, "\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08") }.add(2).deref<2>().get());
    drawScreenEffectMaterial = SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x0C\x8D\x4D\xF8") }.add(1).relativeToAbsolute().get();
    submitReportFunction = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08");
    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    vignette = reinterpret_cast<float*>(SafeAddress{ findPattern(CLIENT_DLL, "\x0F\x11\x05????\xF3\x0F\x7E\x87") }.add(3).deref().add(4).get());
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x10\x53\x8B\x5D\x08\x57\x8B\xF9"));
    predictionRandomSeed = reinterpret_cast<int*>(SafeAddress{ findPattern(CLIENT_DLL, "\x8B\x0D????\xBA????\xE8????\x83\xC4\x04") }.add(2).deref().get());
    moveData = reinterpret_cast<MoveData*>(SafeAddress{ findPattern(CLIENT_DLL, "\xA1????\xF3\x0F\x59\xCD") }.add(1).deref<2>().get());
    moveHelper = reinterpret_cast<MoveHelper*>(SafeAddress{ findPattern(CLIENT_DLL, "\x8B\x0D????\x8B\x45?\x51\x8B\xD4\x89\x02\x8B\x01") }.add(2).deref<2>().get());
    keyValuesFromString = SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x04\x89\x45\xD8") }.add(1).relativeToAbsolute().get();
    keyValuesFindKey = reinterpret_cast<decltype(keyValuesFindKey)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\xF7\x45") }.add(1).relativeToAbsolute().get());
    keyValuesSetString = reinterpret_cast<decltype(keyValuesSetString)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x89\x77\x38") }.add(1).relativeToAbsolute().get());
    getPlayerViewmodelArmConfigForPlayerModel = reinterpret_cast<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x89\x87????\x6A") }.add(1).relativeToAbsolute().get());
    getEventDescriptor = reinterpret_cast<decltype(getEventDescriptor)>(SafeAddress{ findPattern(ENGINE_DLL, "\xE8????\x8B\xD8\x85\xDB\x75\x27") }.add(1).relativeToAbsolute().get());
    activeChannels = reinterpret_cast<ActiveChannels*>(SafeAddress{ findPattern(ENGINE_DLL, "\x8B\x1D????\x89\x5C\x24\x48") }.add(2).deref().get());
    channels = reinterpret_cast<Channel*>(SafeAddress{ findPattern(ENGINE_DLL, "\x81\xC2????\x8B\x72\x54") }.add(2).deref().get());
    playerResource = reinterpret_cast<PlayerResource**>(SafeAddress{ findPattern(CLIENT_DLL, "\x74\x30\x8B\x35????\x85\xF6") }.add(4).deref().get());
    getDecoratedPlayerName = reinterpret_cast<decltype(getDecoratedPlayerName)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x66\x83\x3E") }.add(1).relativeToAbsolute().get());
    scopeDust = SafeAddress{ findPattern(CLIENT_DLL, "\xFF\x50\x3C\x8B\x4C\x24\x20") }.add(3).get();
    scopeArc = findPattern(CLIENT_DLL, "\x8B\x0D????\xFF\xB7????\x8B\x01\xFF\x90????\x8B\x7C\x24\x1C");
    demoOrHLTV = findPattern(CLIENT_DLL, "\x84\xC0\x75\x09\x38\x05");
    money = findPattern(CLIENT_DLL, "\x84\xC0\x75\x0C\x5B");
    demoFileEndReached = findPattern(CLIENT_DLL, "\x8B\xC8\x85\xC9\x74\x1F\x80\x79\x10");
    plantedC4s = reinterpret_cast<decltype(plantedC4s)>(SafeAddress{ findPattern(CLIENT_DLL, "\x7E\x2C\x8B\x15") }.add(4).deref().get());
    gameRules = reinterpret_cast<Entity**>(SafeAddress{ findPattern(CLIENT_DLL, "\x8B\xEC\x8B\x0D????\x85\xC9\x74\x07") }.add(4).deref().get());
    registeredPanoramaEvents = reinterpret_cast<decltype(registeredPanoramaEvents)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\xA1????\xA8\x01\x75\x21") }.add(6).deref().add(-36).get());
    makePanoramaSymbolFn = reinterpret_cast<decltype(makePanoramaSymbolFn)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x45\x0E\x8D\x4D\x0E") }.add(1).relativeToAbsolute().get());
    inventoryManager = reinterpret_cast<InventoryManager*>(SafeAddress{ findPattern(CLIENT_DLL, "\x8D\x44\x24\x28\xB9????\x50") }.add(5).deref().get());
    createEconItemSharedObject = reinterpret_cast<decltype(createEconItemSharedObject)>(SafeAddress{ findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x1C\x8D\x45\xE4\xC7\x45") }.add(20).deref().get());
    addEconItem = reinterpret_cast<decltype(addEconItem)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x84\xC0\x74\xE7") }.add(1).relativeToAbsolute().get());
    clearInventoryImageRGBA = reinterpret_cast<decltype(clearInventoryImageRGBA)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x81\xEC????\x57\x8B\xF9\xC7\x47"));
    panoramaMarshallHelper = *reinterpret_cast<decltype(panoramaMarshallHelper)*>(SafeAddress{ findPattern(CLIENT_DLL, "\x68????\x8B\xC8\xE8????\x8D\x4D\xF4\xFF\x15????\x8B\xCF\xFF\x15????\x5F\x5E\x8B\xE5\x5D\xC3") }.add(1).get());

    findOrCreateEconItemViewForItemID = reinterpret_cast<decltype(findOrCreateEconItemViewForItemID)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\xCE\x83\xC4\x08") }.add(1).relativeToAbsolute().get());
    getInventoryItemByItemID = reinterpret_cast<decltype(getInventoryItemByItemID)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x33\x8B\xD0") }.add(1).relativeToAbsolute().get());
    getSOCData = reinterpret_cast<decltype(getSOCData)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x32\xC9") }.add(1).relativeToAbsolute().get());
    setCustomName = reinterpret_cast<decltype(setCustomName)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x46\x78\xC1\xE8\x0A\xA8\x01\x74\x13\x8B\x46\x34") }.add(1).relativeToAbsolute().get());
    setDynamicAttributeValueFn = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x3C\x53\x8B\x5D\x08\x56\x57\x6A");
    createBaseTypeCache = reinterpret_cast<decltype(createBaseTypeCache)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8D\x4D\x0F") }.add(1).relativeToAbsolute().get());
    uiComponentInventory = reinterpret_cast<void**>(SafeAddress{ findPattern(CLIENT_DLL, "\xC6\x44\x24??\x83\x3D") }.add(7).deref().get());
    setItemSessionPropertyValue = reinterpret_cast<decltype(setItemSessionPropertyValue)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x4C\x24\x2C\x46") }.add(1).relativeToAbsolute().get());
    removeDynamicAttribute = reinterpret_cast<decltype(removeDynamicAttribute)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x08\x8B\xC1\x89\x45\xF8"));

    localPlayer.init(reinterpret_cast<Entity**>(SafeAddress{ findPattern(CLIENT_DLL, "\xA1????\x89\x45\xBC\x85\xC0") }.add(1).deref().get()));

    keyValuesSystem = reinterpret_cast<KeyValuesSystem* (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
    keyValuesAllocEngine = SafeAddress{ findPattern(ENGINE_DLL, "\xE8????\x83\xC4\x08\x84\xC0\x75\x10\xFF\x75\x0C") }.add(1).relativeToAbsolute().add(0x4A).get();
    keyValuesAllocClient = SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x08\x84\xC0\x75\x10") }.add(1).relativeToAbsolute().add(0x3E).get();

    shouldDrawFogReturnAddress = SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x0D????\x0F\xB6\xD0") }.add(1).relativeToAbsolute().add(82).get();
#else
    const auto tier0 = dlopen(TIER0_DLL, RTLD_NOLOAD | RTLD_NOW);
    debugMsg = decltype(debugMsg)(dlsym(tier0, "Msg"));
    conColorMsg = decltype(conColorMsg)(dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz"));
    dlclose(tier0);

    globalVars = reinterpret_cast<GlobalVars*>(SafeAddress{ (*reinterpret_cast<std::uintptr_t**>(clientInterface))[11] + 16 }.relativeToAbsolute().deref().get());
    itemSystem = reinterpret_cast<decltype(itemSystem)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x4D\x63\xEC") }.add(1).relativeToAbsolute().get());

    isOtherEnemy = reinterpret_cast<decltype(isOtherEnemy)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x84\xC0\x44\x89\xE2") }.add(1).relativeToAbsolute().get());
    lineGoesThroughSmoke = reinterpret_cast<decltype(lineGoesThroughSmoke)>(findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x53\x48\x83\xEC\x30\x66\x0F\xD6\x45\xD0"));
    getDecoratedPlayerName = reinterpret_cast<decltype(getDecoratedPlayerName)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x33\x4C\x89\xF7") }.add(1).relativeToAbsolute().get());

    hud = reinterpret_cast<decltype(hud)>(SafeAddress{ findPattern(CLIENT_DLL, "\x53\x48\x8D\x3D????\x48\x83\xEC\x10\xE8") }.add(4).relativeToAbsolute().get());
    findHudElement = reinterpret_cast<decltype(findHudElement)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x48\x8D\x50\xE0") }.add(1).relativeToAbsolute().get());

    disablePostProcessing = reinterpret_cast<decltype(disablePostProcessing)>(SafeAddress{ findPattern(CLIENT_DLL, "\x80\x3D?????\x89\xB5") }.add(2).relativeToAbsolute().get());
    submitReportFunction = findPattern(CLIENT_DLL, "\x55\x48\x89\xF7\x48\x89\xE5\x41\x57\x41\x56\x41\x55\x41\x54\x53\x48\x89\xD3\x48\x83\xEC\x58");
    loadSky = reinterpret_cast<decltype(loadSky)>(SafeAddress{ findPattern(ENGINE_DLL, "\xE8????\x84\xC0\x74\xAB") }.add(1).relativeToAbsolute().get());
    clientMode = reinterpret_cast<decltype(clientMode)>(SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[10] }.add(12).relativeToAbsolute().add(4).relativeToAbsolute().deref().get());
    input = reinterpret_cast<Input*>(SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[16] }.add(3).relativeToAbsolute().deref<2>().get());
    playerResource = reinterpret_cast<PlayerResource**>(SafeAddress{ findPattern(CLIENT_DLL, "\x74\x38\x48\x8B\x3D????\x89\xDE") }.add(5).relativeToAbsolute().get());

    glowObjectManager = reinterpret_cast<decltype(glowObjectManager)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xE7\x8B\x70\x20") }.add(1).relativeToAbsolute().add(12).relativeToAbsolute().get());
    setClanTag = reinterpret_cast<decltype(setClanTag)>(SafeAddress{ findPattern(ENGINE_DLL, "\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0") }.add(1).relativeToAbsolute().get());
    getEventDescriptor = reinterpret_cast<decltype(getEventDescriptor)>(SafeAddress{ findPattern(ENGINE_DLL, "\xE8????\x48\x85\xC0\x74\x62") }.add(1).relativeToAbsolute().get());
    activeChannels = reinterpret_cast<ActiveChannels*>(SafeAddress{ findPattern(ENGINE_DLL, "\x48\x8D\x3D????\x4C\x89\xE6\xE8????\x8B\xBD") }.add(3).relativeToAbsolute().get());
    channels = reinterpret_cast<Channel*>(SafeAddress{ findPattern(ENGINE_DLL, "\x4C\x8D\x35????\x49\x83\xC4\x04") }.add(3).relativeToAbsolute().get());
    cameraThink = SafeAddress{ findPattern(CLIENT_DLL, "\xFF\x90????\x85\xC0\x75\x64") }.add(6).get();
    getPlayerViewmodelArmConfigForPlayerModel = reinterpret_cast<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x48\x8B\x78\x08\xBA") }.add(1).relativeToAbsolute().get());
    keyValuesFromString = reinterpret_cast<decltype(keyValuesFromString)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x48\x89\xDF\x48\x89\x45\xE0") }.add(1).relativeToAbsolute().get());
    keyValuesFindKey = reinterpret_cast<decltype(keyValuesFindKey)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x48\x85\xC0\x75\x24") }.add(1).relativeToAbsolute().get());
    keyValuesSetString = reinterpret_cast<decltype(keyValuesSetString)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xE6\x4C\x89\xFF\xE8????\x48\x8B\x03") }.add(1).relativeToAbsolute().get());
    drawScreenEffectMaterial = findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x57\x41\x56\x45\x89\xC6\x41\x55\x41\x54\x53");
    vignette = reinterpret_cast<float*>(SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8B\x07\x0F\x2F\x05") }.add(6).relativeToAbsolute().get());
    viewRender = reinterpret_cast<ViewRender*>(SafeAddress{ findPattern(CLIENT_DLL, "\x0F\x85????\x48\x8B\x05????\x45\x89\xF8") }.add(9).relativeToAbsolute().deref<2>().get());
    scopeDust = findPattern(CLIENT_DLL, "\x8B\x85????\x43\x8D\x14\x2E");
    scopeArc = findPattern(CLIENT_DLL, "\x49\x8B\x3C\x24\x8B\xB3????\x48\x8B\x07\xFF\x90????\x49\x8B\x3C\x24\x4C\x89\xEA");
    demoOrHLTV = SafeAddress{ findPattern(CLIENT_DLL, "\x0F\xB6\x10\x89\xD0") }.add(-16).get();
    money = findPattern(CLIENT_DLL, "\x84\xC0\x75\x9E\xB8????\xEB\xB9");
    demoFileEndReached = findPattern(CLIENT_DLL, "\x48\x85\xC0\x0F\x84????\x80\x78\x10?\x74\x7F");
    clearHudWeapon = reinterpret_cast<decltype(clearHudWeapon)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\xC6\x45\xB8\x01") }.add(1).relativeToAbsolute().get());
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x49\x89\xF4\x53\x48\x89\xFB\x48\x83\xEC\x10\x48\x8B\x07"));
    setAbsOrigin = reinterpret_cast<decltype(setAbsOrigin)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x49\x8B\x07\x31\xF6") }.add(1).relativeToAbsolute().get());
    plantedC4s = reinterpret_cast<decltype(plantedC4s)>(SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8D\x3D????\x42\xC6\x44\x28") }.add(3).relativeToAbsolute().get());
    gameRules = reinterpret_cast<Entity**>(SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8B\x1D????\x48\x8B\x3B\x48\x85\xFF\x74\x06") }.add(3).relativeToAbsolute().deref().get());
    dispatchSound = reinterpret_cast<int*>(SafeAddress{ findPattern(ENGINE_DLL, "\x74\x10\xE8????\x48\x8B\x35") }.add(3).get());
    predictionRandomSeed = reinterpret_cast<int*>(SafeAddress{ findPattern(CLIENT_DLL, "\x41\x8D\x56\xFF\x31\xC9") }.add(-14).relativeToAbsolute().deref().get());
    registeredPanoramaEvents = reinterpret_cast<decltype(registeredPanoramaEvents)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x8B\x50\x10\x49\x89\xC6") }.add(1).relativeToAbsolute().add(12).relativeToAbsolute().get());
    makePanoramaSymbolFn = reinterpret_cast<decltype(makePanoramaSymbolFn)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x45\xA0\x31\xF6") }.add(1).relativeToAbsolute().get());
    moveData = reinterpret_cast<MoveData*>(SafeAddress{ findPattern(CLIENT_DLL, "\x4C\x8B\x2D????\x0F\xB6\x93") }.add(3).relativeToAbsolute().deref<2>().get());
    moveHelper = reinterpret_cast<MoveHelper*>(SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8B\x05????\x44\x89\x85????\x48\x8B\x38") }.add(3).relativeToAbsolute().deref<2>().get());

    inventoryManager = reinterpret_cast<decltype(inventoryManager)>(SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8D\x35????\x48\x8D\x3D????\xE9????\x90\x90\x90\x55") }.add(3).relativeToAbsolute().get());
    createEconItemSharedObject = reinterpret_cast<decltype(createEconItemSharedObject)>(SafeAddress{ findPattern(CLIENT_DLL, "\x55\x48\x8D\x05????\x31\xD2\x4C\x8D\x0D") }.add(50).relativeToAbsolute().get());
    addEconItem = reinterpret_cast<decltype(addEconItem)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x45\x3B\x65\x28\x72\xD6") }.add(1).relativeToAbsolute().get());
    clearInventoryImageRGBA = reinterpret_cast<decltype(clearInventoryImageRGBA)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x83\xC3\x01\x49\x83\xC4\x08\x41\x3B\x5D\x50") }.add(1).relativeToAbsolute().get());
    panoramaMarshallHelper = reinterpret_cast<decltype(panoramaMarshallHelper)>(SafeAddress{ findPattern(CLIENT_DLL, "\xF3\x0F\x11\x05????\x48\x89\x05????\x48\xC7\x05????????\xC7\x05") }.add(11).relativeToAbsolute().get());
    findOrCreateEconItemViewForItemID = reinterpret_cast<decltype(findOrCreateEconItemViewForItemID)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xEF\x48\x89\x45\xC8") }.add(1).relativeToAbsolute().get());
    getInventoryItemByItemID = reinterpret_cast<decltype(getInventoryItemByItemID)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x45\x84\xED\x49\x89\xC1") }.add(1).relativeToAbsolute().get());
    getSOCData = reinterpret_cast<decltype(getSOCData)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x5B\x44\x89\xEE") }.add(1).relativeToAbsolute().get());
    setCustomName = reinterpret_cast<decltype(setCustomName)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x41\x8B\x84\x24????\xE9????\x8B\x98") }.add(1).relativeToAbsolute().get());
    setDynamicAttributeValueFn = SafeAddress{ findPattern(CLIENT_DLL, "\x41\x8B\x06\x49\x8D\x7D\x08") }.add(-95).get();
    createBaseTypeCache = reinterpret_cast<decltype(createBaseTypeCache)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x48\x89\xDE\x5B\x48\x8B\x10") }.add(1).relativeToAbsolute().get());
    insertIntoTree = SafeAddress{ findPattern(CLIENT_DLL, "\x74\x24\x4C\x8B\x10") }.add(31).get();
    uiComponentInventory = reinterpret_cast<decltype(uiComponentInventory)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x4C\x89\x3D????\x4C\x89\xFF\xEB\x9E") }.add(8).relativeToAbsolute().get());
    setItemSessionPropertyValue = reinterpret_cast<decltype(setItemSessionPropertyValue)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x48\x8B\x85????\x41\x83\xC4\x01") }.add(1).relativeToAbsolute().get());

    removeDynamicAttribute = reinterpret_cast<decltype(removeDynamicAttribute)>(SafeAddress{ findPattern(CLIENT_DLL, "\xE8????\x80\x3D?????\x75\x14\x48\x8D\x3D????\xE8????\x85\xC0\x0F\x85????\xC7\x45") }.add(1).relativeToAbsolute().get());

    localPlayer.init(reinterpret_cast<Entity**>(SafeAddress{ findPattern(CLIENT_DLL, "\x83\xFF\xFF\x48\x8B\x05") }.add(6).relativeToAbsolute().get()));
#endif
}
