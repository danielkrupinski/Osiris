#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <string_view>
#include <utility>

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#elif __linux__
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "Interfaces.h"
#include "Memory.h"
#include "SDK/LocalPlayer.h"

template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

struct ModuleInfo {
    void* base;
    std::size_t size;
};

static ModuleInfo getModuleInformation(const char* name) noexcept
{
#ifdef _WIN32
    if (HMODULE handle = GetModuleHandleA(name)) {
        if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo)))
            return ModuleInfo{ moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage };
    }
    return {};
#elif __linux__
    struct ModuleInfo_ {
        const char* name;
        void* base = nullptr;
        std::size_t size = 0;
    } moduleInfo;

    moduleInfo.name = name;

    dl_iterate_phdr([](struct dl_phdr_info* info, std::size_t, void* data) {
        const auto moduleInfo = reinterpret_cast<ModuleInfo_*>(data);
        if (!std::string_view{ info->dlpi_name }.ends_with(moduleInfo->name))
            return 0;

        if (const auto fd = open(info->dlpi_name, O_RDONLY); fd >= 0) {
            if (struct stat st; fstat(fd, &st) == 0) {
                if (const auto map = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0); map != MAP_FAILED) {
                    const auto ehdr = (ElfW(Ehdr)*)map;
                    const auto shdrs = (ElfW(Shdr)*)(std::uintptr_t(ehdr) + ehdr->e_shoff);
                    const auto strTab = (const char*)(std::uintptr_t(ehdr) + shdrs[ehdr->e_shstrndx].sh_offset);

                    for (auto i = 0; i < ehdr->e_shnum; ++i) {
                        const auto shdr = (ElfW(Shdr)*)(std::uintptr_t(shdrs) + i * ehdr->e_shentsize);

                        if (std::strcmp(strTab + shdr->sh_name, ".text") != 0)
                            continue;

                        moduleInfo->base = (void*)(info->dlpi_addr + shdr->sh_offset);
                        moduleInfo->size = shdr->sh_size;
                        munmap(map, st.st_size);
                        close(fd);
                        return 1;
                    }
                    munmap(map, st.st_size);
                }
            }
            close(fd);
        }

        moduleInfo->base = (void*)(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr);
        moduleInfo->size = info->dlpi_phdr[0].p_memsz;
        return 1;
    }, &moduleInfo);

    return ModuleInfo{ moduleInfo.base, moduleInfo.size };
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
static std::uintptr_t findPattern(ModuleInfo moduleInfo, std::string_view pattern) noexcept
{
    static auto id = 0;
    ++id;

    if (moduleInfo.base && moduleInfo.size) {
        const auto lastIdx = pattern.length() - 1;
        const auto badCharTable = generateBadCharTable(pattern);

        auto start = static_cast<const char*>(moduleInfo.base);
        const auto end = start + moduleInfo.size - pattern.length();

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

template <bool ReportNotFound = true>
static std::uintptr_t findPattern(const char* moduleName, std::string_view pattern) noexcept
{
    return findPattern<ReportNotFound>(getModuleInformation(moduleName), pattern);
}

Memory::Memory() noexcept
{
#ifdef _WIN32
    present = findPattern("gameoverlayrenderer", "\xFF\x15????\x8B\xF0\x85\xFF") + 2;
    reset = findPattern("gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xD8") + 9;

    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces->client))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[11] + 10);
    glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern(CLIENT_DLL, "\x0F\x11\x05????\x83\xC8\x01") + 3);
    disablePostProcessing = *reinterpret_cast<bool**>(findPattern(CLIENT_DLL, "\x83\xEC\x4C\x80\x3D") + 5);
    loadSky = relativeToAbsolute<decltype(loadSky)>(findPattern(ENGINE_DLL, "\xE8????\x84\xC0\x74\x2D\xA1") + 1);
    setClanTag = reinterpret_cast<decltype(setClanTag)>(findPattern(ENGINE_DLL, "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15"));
    lineGoesThroughSmoke = relativeToAbsolute<decltype(lineGoesThroughSmoke)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x4C\x24\x30\x33\xD2") + 1);
    cameraThink = findPattern(CLIENT_DLL, "\x85\xC0\x75\x30\x38\x86");
    getSequenceActivity = reinterpret_cast<decltype(getSequenceActivity)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83"));
    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(findPattern(CLIENT_DLL, "\x8B\xCE\xE8????\x02\xC0") + 3);
    auto temp = reinterpret_cast<std::uintptr_t*>(findPattern(CLIENT_DLL, "\xB9????\xE8????\x8B\x5D\x08") + 1);
    hud = *temp;
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(reinterpret_cast<uintptr_t>(temp) + 5);
    clearHudWeapon = relativeToAbsolute<decltype(clearHudWeapon)>(findPattern(CLIENT_DLL, "\xE8????\x8B\xF0\xC6\x44\x24??\xC6\x44\x24") + 1);
    itemSystem = relativeToAbsolute<decltype(itemSystem)>(findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x0F") + 1);
    setAbsOrigin = relativeToAbsolute<decltype(setAbsOrigin)>(findPattern(CLIENT_DLL, "\xE8????\xEB\x19\x8B\x07") + 1);
    insertIntoTree = findPattern(CLIENT_DLL, "\x56\x52\xFF\x50\x18") + 5;
    dispatchSound = reinterpret_cast<int*>(findPattern(ENGINE_DLL, "\x74\x0B\xE8????\x8B\x3D") + 3);
    traceToExit = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x4C\xF3\x0F\x10\x75");
    viewRender = **reinterpret_cast<ViewRender***>(findPattern(CLIENT_DLL, "\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08") + 2);
    viewRenderBeams = *reinterpret_cast<ViewRenderBeams**>(findPattern(CLIENT_DLL, "\xB9????\x0F\x11\x44\x24?\xC7\x44\x24?????\xF3\x0F\x10\x84\x24") + 1);
    drawScreenEffectMaterial = relativeToAbsolute<uintptr_t>(findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x0C\x8D\x4D\xF8") + 1);
    submitReportFunction = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08");
    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    vignette = *reinterpret_cast<float**>(findPattern(CLIENT_DLL, "\x0F\x11\x05????\xF3\x0F\x7E\x87") + 3) + 1;
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x10\x53\x8B\x5D\x08\x57\x8B\xF9"));
    predictionRandomSeed = *reinterpret_cast<int**>(findPattern(CLIENT_DLL, "\x8B\x0D????\xBA????\xE8????\x83\xC4\x04") + 2);
    moveData = **reinterpret_cast<MoveData***>(findPattern(CLIENT_DLL, "\xA1????\xF3\x0F\x59\xCD") + 1);
    moveHelper = **reinterpret_cast<MoveHelper***>(findPattern(CLIENT_DLL, "\x8B\x0D????\x8B\x45?\x51\x8B\xD4\x89\x02\x8B\x01") + 2);
    keyValuesFromString = relativeToAbsolute<decltype(keyValuesFromString)>(findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x04\x89\x45\xD8") + 1);
    keyValuesFindKey = relativeToAbsolute<decltype(keyValuesFindKey)>(findPattern(CLIENT_DLL, "\xE8????\xF7\x45") + 1);
    keyValuesSetString = relativeToAbsolute<decltype(keyValuesSetString)>(findPattern(CLIENT_DLL, "\xE8????\x89\x77\x38") + 1);
    weaponSystem = *reinterpret_cast<WeaponSystem**>(findPattern(CLIENT_DLL, "\x8B\x35????\xFF\x10\x0F\xB7\xC0") + 2);
    getPlayerViewmodelArmConfigForPlayerModel = relativeToAbsolute<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(findPattern(CLIENT_DLL, "\xE8????\x89\x87????\x6A") + 1);
    getEventDescriptor = relativeToAbsolute<decltype(getEventDescriptor)>(findPattern(ENGINE_DLL, "\xE8????\x8B\xD8\x85\xDB\x75\x27") + 1);
    activeChannels = *reinterpret_cast<ActiveChannels**>(findPattern(ENGINE_DLL, "\x8B\x1D????\x89\x5C\x24\x48") + 2);
    channels = *reinterpret_cast<Channel**>(findPattern(ENGINE_DLL, "\x81\xC2????\x8B\x72\x54") + 2);
    playerResource = *reinterpret_cast<PlayerResource***>(findPattern(CLIENT_DLL, "\x74\x30\x8B\x35????\x85\xF6") + 4);
    getDecoratedPlayerName = relativeToAbsolute<decltype(getDecoratedPlayerName)>(findPattern(CLIENT_DLL, "\xE8????\x66\x83\x3E") + 1);
    scopeDust = findPattern(CLIENT_DLL, "\xFF\x50\x3C\x8B\x4C\x24\x20") + 3;
    scopeArc = findPattern(CLIENT_DLL, "\x8B\x0D????\xFF\xB7????\x8B\x01\xFF\x90????\x8B\x7C\x24\x1C");
    demoOrHLTV = findPattern(CLIENT_DLL, "\x84\xC0\x75\x09\x38\x05");
    money = findPattern(CLIENT_DLL, "\x84\xC0\x75\x0C\x5B");
    demoFileEndReached = findPattern(CLIENT_DLL, "\x8B\xC8\x85\xC9\x74\x1F\x80\x79\x10");
    plantedC4s = *reinterpret_cast<decltype(plantedC4s)*>(findPattern(CLIENT_DLL, "\x7E\x2C\x8B\x15") + 4);
    gameRules = *reinterpret_cast<Entity***>(findPattern(CLIENT_DLL, "\x8B\xEC\x8B\x0D????\x85\xC9\x74\x07") + 4);
    registeredPanoramaEvents = reinterpret_cast<decltype(registeredPanoramaEvents)>(*reinterpret_cast<std::uintptr_t*>(findPattern(CLIENT_DLL, "\xE8????\xA1????\xA8\x01\x75\x21") + 6) - 36);
    makePanoramaSymbolFn = relativeToAbsolute<decltype(makePanoramaSymbolFn)>(findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x45\x0E\x8D\x4D\x0E") + 1);
    inventoryManager = *reinterpret_cast<InventoryManager**>(findPattern(CLIENT_DLL, "\x8D\x44\x24\x28\xB9????\x50") + 5);
    createEconItemSharedObject = *reinterpret_cast<decltype(createEconItemSharedObject)*>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x1C\x8D\x45\xE4\xC7\x45") + 20);
    addEconItem = relativeToAbsolute<decltype(addEconItem)>(findPattern(CLIENT_DLL, "\xE8????\x84\xC0\x74\xE7") + 1);
    clearInventoryImageRGBA = reinterpret_cast<decltype(clearInventoryImageRGBA)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x81\xEC????\x57\x8B\xF9\xC7\x47"));
    panoramaMarshallHelper = *reinterpret_cast<decltype(panoramaMarshallHelper)*>(findPattern(CLIENT_DLL, "\x68????\x8B\xC8\xE8????\x8D\x4D\xF4\xFF\x15????\x8B\xCF\xFF\x15????\x5F\x5E\x8B\xE5\x5D\xC3") + 1);
    setStickerToolSlotGetArgAsNumberReturnAddress = findPattern(CLIENT_DLL, "\xFF\xD2\xDD\x5C\x24\x10\xF2\x0F\x2C\x7C\x24") + 2;
    setStickerToolSlotGetArgAsStringReturnAddress = setStickerToolSlotGetArgAsNumberReturnAddress - 49;
    wearItemStickerGetArgAsNumberReturnAddress = findPattern(CLIENT_DLL, "\xDD\x5C\x24\x18\xF2\x0F\x2C\x7C\x24?\x85\xFF");
    wearItemStickerGetArgAsStringReturnAddress = wearItemStickerGetArgAsNumberReturnAddress - 80;
    setNameToolStringGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x8B\xF8\xC6\x45\x08?\x33\xC0");
    clearCustomNameGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\xFF\x50\x1C\x8B\xF0\x85\xF6\x74\x21") + 3;
    deleteItemGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x85\xC0\x74\x22\x51");
    setStatTrakSwapToolItemsGetArgAsStringReturnAddress1 = findPattern(CLIENT_DLL, "\x85\xC0\x74\x7E\x8B\xC8\xE8????\x8B\x37");
    setStatTrakSwapToolItemsGetArgAsStringReturnAddress2 = setStatTrakSwapToolItemsGetArgAsStringReturnAddress1 + 44;
    acknowledgeNewItemByItemIDGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x85\xC0\x74\x33\x8B\xC8\xE8????\xB9");

    findOrCreateEconItemViewForItemID = relativeToAbsolute<decltype(findOrCreateEconItemViewForItemID)>(findPattern(CLIENT_DLL, "\xE8????\x8B\xCE\x83\xC4\x08") + 1);
    getInventoryItemByItemID = relativeToAbsolute<decltype(getInventoryItemByItemID)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x33\x8B\xD0") + 1);
    useToolGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x85\xC0\x0F\x84????\x8B\xC8\xE8????\x8B\x37");
    useToolGetArg2AsStringReturnAddress = useToolGetArgAsStringReturnAddress + 52;
    getSOCData = relativeToAbsolute<decltype(getSOCData)>(findPattern(CLIENT_DLL, "\xE8????\x32\xC9") + 1);
    setCustomName = relativeToAbsolute<decltype(setCustomName)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x46\x78\xC1\xE8\x0A\xA8\x01\x74\x13\x8B\x46\x34") + 1);
    setDynamicAttributeValueFn = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x3C\x53\x8B\x5D\x08\x56\x57\x6A");
    createBaseTypeCache = relativeToAbsolute<decltype(createBaseTypeCache)>(findPattern(CLIENT_DLL, "\xE8????\x8D\x4D\x0F") + 1);
    uiComponentInventory = *reinterpret_cast<void***>(findPattern(CLIENT_DLL, "\xC6\x44\x24??\x83\x3D") + 7);
    setItemSessionPropertyValue = relativeToAbsolute<decltype(setItemSessionPropertyValue)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x4C\x24\x2C\x46") + 1);

    localPlayer.init(*reinterpret_cast<Entity***>(findPattern(CLIENT_DLL, "\xA1????\x89\x45\xBC\x85\xC0") + 1));

    keyValuesSystem = reinterpret_cast<KeyValuesSystem* (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
    keyValuesAllocEngine = findPattern(ENGINE_DLL, "\xFF\x52\x04\x85\xC0\x74\x0C\x56") + 3;
    keyValuesAllocClient = findPattern(CLIENT_DLL, "\xFF\x52\x04\x85\xC0\x74\x0C\x56") + 3;
#else
    const auto tier0 = dlopen(TIER0_DLL, RTLD_NOLOAD | RTLD_NOW);
    debugMsg = decltype(debugMsg)(dlsym(tier0, "Msg"));
    conColorMsg = decltype(conColorMsg)(dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz"));
    dlclose(tier0);

    const auto libSDL = dlopen("libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);
    pollEvent = relativeToAbsolute<uintptr_t>(uintptr_t(dlsym(libSDL, "SDL_PollEvent")) + 2);
    swapWindow = relativeToAbsolute<uintptr_t>(uintptr_t(dlsym(libSDL, "SDL_GL_SwapWindow")) + 2);
    dlclose(libSDL);

    globalVars = *relativeToAbsolute<GlobalVars**>((*reinterpret_cast<std::uintptr_t**>(interfaces->client))[11] + 16);
    itemSystem = relativeToAbsolute<decltype(itemSystem)>(findPattern(CLIENT_DLL, "\xE8????\x4D\x63\xEC") + 1);
    weaponSystem = *relativeToAbsolute<WeaponSystem**>(findPattern(CLIENT_DLL, "\x48\x8B\x58\x10\x48\x8B\x07\xFF\x10") + 12);

    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(findPattern(CLIENT_DLL, "\xE8????\x84\xC0\x44\x89\xE2") + 1);
    lineGoesThroughSmoke = reinterpret_cast<decltype(lineGoesThroughSmoke)>(findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x53\x48\x83\xEC\x30\x66\x0F\xD6\x45\xD0"));
    getDecoratedPlayerName = relativeToAbsolute<decltype(getDecoratedPlayerName)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x33\x4C\x89\xF7") + 1);

    hud = relativeToAbsolute<decltype(hud)>(findPattern(CLIENT_DLL, "\x53\x48\x8D\x3D????\x48\x83\xEC\x10\xE8") + 4);
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(findPattern(CLIENT_DLL, "\xE8????\x48\x8D\x50\xE0") + 1);

    disablePostProcessing = relativeToAbsolute<decltype(disablePostProcessing)>(findPattern(CLIENT_DLL, "\x80\x3D?????\x89\xB5") + 2);
    submitReportFunction = findPattern(CLIENT_DLL, "\x55\x48\x89\xF7\x48\x89\xE5\x41\x57\x41\x56\x41\x55\x41\x54\x53\x48\x89\xD3\x48\x83\xEC\x58");
    loadSky = relativeToAbsolute<decltype(loadSky)>(findPattern(ENGINE_DLL, "\xE8????\x84\xC0\x74\xAB") + 1);
    clientMode = *relativeToAbsolute<decltype(clientMode)*>(relativeToAbsolute<uintptr_t>((*reinterpret_cast<uintptr_t**>(interfaces->client))[10] + 12) + 4);
    input = **relativeToAbsolute<Input***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[16] + 3);
    playerResource = relativeToAbsolute<PlayerResource**>(findPattern(CLIENT_DLL, "\x74\x38\x48\x8B\x3D????\x89\xDE") + 5);

    glowObjectManager = relativeToAbsolute<decltype(glowObjectManager)>(relativeToAbsolute<uintptr_t>(findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xE7\x8B\x70\x20") + 1) + 12);
    setClanTag = relativeToAbsolute<decltype(setClanTag)>(findPattern(ENGINE_DLL, "\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0") + 1);
    getEventDescriptor = relativeToAbsolute<decltype(getEventDescriptor)>(findPattern(ENGINE_DLL, "\xE8????\x48\x85\xC0\x74\x62") + 1);
    activeChannels = relativeToAbsolute<ActiveChannels*>(findPattern(ENGINE_DLL, "\x48\x8D\x3D????\x4C\x89\xE6\xE8????\x8B\xBD") + 3);
    channels = relativeToAbsolute<Channel*>(findPattern(ENGINE_DLL, "\x4C\x8D\x35????\x49\x83\xC4\x04") + 3);
    cameraThink = findPattern(CLIENT_DLL, "\xFF\x90????\x85\xC0\x75\x64") + 6;
    getPlayerViewmodelArmConfigForPlayerModel = relativeToAbsolute<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(findPattern(CLIENT_DLL, "\xE8????\x48\x8B\x78\x08\xBA") + 1);
    keyValuesFromString = relativeToAbsolute<decltype(keyValuesFromString)>(findPattern(CLIENT_DLL, "\xE8????\x48\x89\xDF\x48\x89\x45\xE0") + 1);
    keyValuesFindKey = relativeToAbsolute<decltype(keyValuesFindKey)>(findPattern(CLIENT_DLL, "\xE8????\x48\x85\xC0\x75\x24") + 1);
    keyValuesSetString = relativeToAbsolute<decltype(keyValuesSetString)>(findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xE6\x4C\x89\xFF\xE8????\x48\x8B\x03") + 1);
    drawScreenEffectMaterial = findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x57\x41\x56\x45\x89\xC6\x41\x55\x41\x54\x53");
    vignette = relativeToAbsolute<float*>(findPattern(CLIENT_DLL, "\x48\x8B\x07\x0F\x2F\x05") + 6);
    viewRender = **relativeToAbsolute<ViewRender***>(findPattern(CLIENT_DLL, "\x0F\x85????\x48\x8B\x05????\x45\x89\xF8") + 9);
    viewRenderBeams = **relativeToAbsolute<ViewRenderBeams***>(findPattern(CLIENT_DLL, "\x4C\x89\xF6\x4C\x8B\x25????\x48\x8D\x05") + 6);
    scopeDust = findPattern(CLIENT_DLL, "\x8B\x85????\x43\x8D\x14\x2E");
    scopeArc = findPattern(CLIENT_DLL, "\x49\x8B\x3C\x24\x8B\xB3????\x48\x8B\x07\xFF\x90????\x49\x8B\x3C\x24\x4C\x89\xEA");
    demoOrHLTV = findPattern(CLIENT_DLL, "\x0F\xB6\x10\x89\xD0") - 16;
    money = findPattern(CLIENT_DLL, "\x84\xC0\x75\x9E\xB8????\xEB\xB9");
    demoFileEndReached = findPattern(CLIENT_DLL, "\x48\x85\xC0\x0F\x84????\x80\x78\x10?\x74\x7F");
    clearHudWeapon = relativeToAbsolute<decltype(clearHudWeapon)>(findPattern(CLIENT_DLL, "\xE8????\xC6\x45\xB8\x01") + 1);
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x49\x89\xF4\x53\x48\x89\xFB\x48\x83\xEC\x10\x48\x8B\x07"));
    setAbsOrigin = relativeToAbsolute<decltype(setAbsOrigin)>(findPattern(CLIENT_DLL, "\xE8????\x49\x8B\x07\x31\xF6") + 1);
    plantedC4s = relativeToAbsolute<decltype(plantedC4s)>(findPattern(CLIENT_DLL, "\x48\x8D\x3D????\x42\xC6\x44\x28") + 3);
    gameRules = *relativeToAbsolute<Entity***>(findPattern(CLIENT_DLL, "\x48\x8B\x1D????\x48\x8B\x3B\x48\x85\xFF\x74\x06") + 3);
    dispatchSound = reinterpret_cast<int*>(findPattern(ENGINE_DLL, "\x74\x10\xE8????\x48\x8B\x35") + 3);
    predictionRandomSeed = *relativeToAbsolute<int**>(findPattern(CLIENT_DLL, "\x41\x8D\x56\xFF\x31\xC9") - 14);
    registeredPanoramaEvents = relativeToAbsolute<decltype(registeredPanoramaEvents)>(relativeToAbsolute<std::uintptr_t>(findPattern(CLIENT_DLL, "\xE8????\x8B\x50\x10\x49\x89\xC6") + 1) + 12);
    makePanoramaSymbolFn = relativeToAbsolute<decltype(makePanoramaSymbolFn)>(findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x45\xA0\x31\xF6") + 1);
    moveData = **relativeToAbsolute<MoveData***>(findPattern(CLIENT_DLL, "\x4C\x8B\x2D????\x0F\xB6\x93") + 3);
    moveHelper = **relativeToAbsolute<MoveHelper***>(findPattern(CLIENT_DLL, "\x48\x8B\x05????\x44\x89\x85????\x48\x8B\x38") + 3);

    inventoryManager = relativeToAbsolute<decltype(inventoryManager)>(findPattern(CLIENT_DLL, "\x48\x8D\x35????\x48\x8D\x3D????\xE9????\x90\x90\x90\x55") + 3);
    createEconItemSharedObject = relativeToAbsolute<decltype(createEconItemSharedObject)>(findPattern(CLIENT_DLL, "\x55\x48\x8D\x05????\x31\xD2\x4C\x8D\x0D") + 50);
    addEconItem = relativeToAbsolute<decltype(addEconItem)>(findPattern(CLIENT_DLL, "\xE8????\x45\x3B\x65\x28\x72\xD6") + 1);
    clearInventoryImageRGBA = relativeToAbsolute<decltype(clearInventoryImageRGBA)>(findPattern(CLIENT_DLL, "\xE8????\x83\xC3\x01\x49\x83\xC4\x08\x41\x3B\x5D\x50") + 1);
    panoramaMarshallHelper = relativeToAbsolute<decltype(panoramaMarshallHelper)>(findPattern(CLIENT_DLL, "\xF3\x0F\x11\x05????\x48\x89\x05????\x48\xC7\x05????????\xC7\x05") + 11);
    setStickerToolSlotGetArgAsNumberReturnAddress = findPattern(CLIENT_DLL, "\xF2\x44\x0F\x2C\xF0\x45\x85\xF6\x78\x32");
    setStickerToolSlotGetArgAsStringReturnAddress = setStickerToolSlotGetArgAsNumberReturnAddress - 46;
    findOrCreateEconItemViewForItemID = relativeToAbsolute<decltype(findOrCreateEconItemViewForItemID)>(findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xEF\x48\x89\x45\xC8") + 1);
    getInventoryItemByItemID = relativeToAbsolute<decltype(getInventoryItemByItemID)>(findPattern(CLIENT_DLL, "\xE8????\x45\x84\xED\x49\x89\xC1") + 1);
    getSOCData = relativeToAbsolute<decltype(getSOCData)>(findPattern(CLIENT_DLL, "\xE8????\x5B\x44\x89\xEE") + 1);
    setCustomName = relativeToAbsolute<decltype(setCustomName)>(findPattern(CLIENT_DLL, "\xE8????\x41\x8B\x84\x24????\xE9????\x8B\x98") + 1);
    useToolGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xDA\x48\x89\xC7\xE8????\x48\x8B\x0B");
    useToolGetArg2AsStringReturnAddress = useToolGetArgAsStringReturnAddress + 55;
    wearItemStickerGetArgAsNumberReturnAddress = findPattern(CLIENT_DLL, "\xF2\x44\x0F\x2C\xF8\x45\x39\xFE");
    wearItemStickerGetArgAsStringReturnAddress = wearItemStickerGetArgAsNumberReturnAddress - 57;
    setNameToolStringGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\xBA????\x4C\x89\xF6\x48\x89\xC7\x49\x89\xC4");
    clearCustomNameGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xE5\x48\x89\xC7\xE8????\x49\x89\xC4");
    deleteItemGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xDE\x48\x89\xC7\xE8????\x48\x89\xC3\xE8????\x48\x89\xDE");
    setDynamicAttributeValueFn = findPattern(CLIENT_DLL, "\x41\x8B\x06\x49\x8D\x7D\x08") - 95;
    createBaseTypeCache = relativeToAbsolute<decltype(createBaseTypeCache)>(findPattern(CLIENT_DLL, "\xE8????\x48\x89\xDE\x5B\x48\x8B\x10") + 1);
    insertIntoTree = findPattern(CLIENT_DLL, "\x74\x24\x4C\x8B\x10") + 31;
    uiComponentInventory = relativeToAbsolute<decltype(uiComponentInventory)>(findPattern(CLIENT_DLL, "\xE8????\x4C\x89\x3D????\x4C\x89\xFF\xEB\x9E") + 8);
    setItemSessionPropertyValue = relativeToAbsolute<decltype(setItemSessionPropertyValue)>(findPattern(CLIENT_DLL, "\xE8????\x48\x8B\x85????\x41\x83\xC4\x01") + 1);

    setStatTrakSwapToolItemsGetArgAsStringReturnAddress1 = findPattern(CLIENT_DLL, "\x74\x84\x4C\x89\xEE\x4C\x89\xF7\xE8????\x48\x85\xC0") - 86;
    setStatTrakSwapToolItemsGetArgAsStringReturnAddress2 = setStatTrakSwapToolItemsGetArgAsStringReturnAddress1 + 55;
    acknowledgeNewItemByItemIDGetArgAsStringReturnAddress = findPattern(CLIENT_DLL, "\x48\x89\xC7\xE8????\x4C\x89\xEF\x48\x89\xC6\xE8????\x48\x8B\x0B") - 5;

    localPlayer.init(relativeToAbsolute<Entity**>(findPattern(CLIENT_DLL, "\x83\xFF\xFF\x48\x8B\x05") + 6));
#endif
}
