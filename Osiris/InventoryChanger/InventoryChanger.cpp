#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>
#include <string_view>
#include <type_traits>
#include <utility>

#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "InventoryChanger.h"
#include "../ProtobufReader.h"
#include "../Texture.h"

#include "../nlohmann/json.hpp"

#include "../SDK/ClassId.h"
#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/EconItemView.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FileSystem.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/Panorama.h"
#include "../SDK/PlayerResource.h"
#include "../SDK/Platform.h"
#include "../SDK/WeaponId.h"

#include "../Helpers.h"

#include "Inventory.h"
#include "StaticData.h"
#include "ToolUser.h"

static void addToInventory(const std::unordered_map<std::size_t, int>& toAdd) noexcept
{
    for (const auto [idx, count] : toAdd) {
        for (int i = 0; i < count; ++i)
            Inventory::addItemUnacknowledged(idx, Inventory::INVALID_DYNAMIC_DATA_IDX);
    }
}

static void sendInventoryUpdatedEvent() noexcept
{
    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_MyPersona_InventoryUpdated")); idx != -1) {
        if (const auto eventPtr = memory->registeredPanoramaEvents->memory[idx].value.makeEvent(nullptr))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(eventPtr);
    }
}

static Entity* createGlove(int entry, int serial) noexcept
{
    static std::add_pointer_t<Entity* __CDECL(int, int)> createWearable = nullptr;

    if (!createWearable) {
        createWearable = []() -> decltype(createWearable) {
            for (auto clientClass = interfaces->client->getAllClasses(); clientClass; clientClass = clientClass->next)
                if (clientClass->classId == ClassId::EconWearable)
                    return clientClass->createFunction;
            return nullptr;
        }();
    }

    if (!createWearable)
        return nullptr;

    createWearable(entry, serial);
    return interfaces->entityList->getEntity(entry);
}

static void applyGloves(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto itemView = localInventory.getItemInLoadout(local->getTeamNumber(), 41);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isGlove())
        return;

    const auto wearables = local->wearables();
    static int gloveHandle = 0;

    auto glove = interfaces->entityList->getEntityFromHandle(wearables[0]);
    if (!glove)
        glove = interfaces->entityList->getEntityFromHandle(gloveHandle);

#ifdef _WIN32 // testing new method
    constexpr auto NUM_ENT_ENTRIES = 8192;
    if (!glove)
        glove = createGlove(NUM_ENT_ENTRIES - 1, -1);

    if (!glove)
        return;

    wearables[0] = gloveHandle = glove->handle();
    glove->accountID() = localInventory.getAccountID();
    glove->entityQuality() = 3;
    local->body() = 1;

    bool dataUpdated = false;
    if (auto& definitionIndex = glove->itemDefinitionIndex2(); definitionIndex != item->get().weaponID) {
        definitionIndex = item->get().weaponID;

        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID))
            glove->setModelIndex(interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel()));

        dataUpdated = true;
    }

    if (glove->itemID() != soc->itemID) {
        glove->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        glove->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
        dataUpdated = true;
    }

    glove->initialized() = true;
    memory->equipWearable(glove, local);

    if (dataUpdated) {
        // FIXME: This leaks memory
        glove->econItemView().visualDataProcessors().size = 0;
        glove->econItemView().customMaterials().size = 0;
        //

        glove->postDataUpdate(0);
        glove->onDataChanged(0);
    }

#else // on linux still old method
    if (!glove) {
        const auto entry = interfaces->entityList->getHighestEntityIndex() + 1;
        const auto serial = Helpers::random(0, 0x1000);
        glove = createGlove(entry, serial);
    }

    if (!glove)
        return;

    wearables[0] = gloveHandle = glove->handle();
    glove->accountID() = localInventory.getAccountID();
    glove->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
    glove->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
    glove->entityQuality() = 3;
    glove->initialized() = true;
    memory->equipWearable(glove, local);
    local->body() = 1;

    if (auto& definitionIndex = glove->itemDefinitionIndex2(); definitionIndex != item->get().weaponID) {
        definitionIndex = item->get().weaponID;

        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID)) {
            glove->setModelIndex(interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel()));
            glove->preDataUpdate(0);
        }
    }
#endif
}

static void applyKnife(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localXuid = local->getSteamId();

    const auto itemView = localInventory.getItemInLoadout(local->getTeamNumber(), 0);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isSkin())
        return;

    auto& weapons = local->weapons();

    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (!Helpers::isKnife(definitionIndex))
            continue;

        if (weapon->originalOwnerXuid() != localXuid)
            continue;

        weapon->accountID() = localInventory.getAccountID();
        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
        weapon->entityQuality() = 3;

        if (definitionIndex != item->get().weaponID) {
            definitionIndex = item->get().weaponID;

            if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID)) {
                weapon->setModelIndex(interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel()));
                weapon->preDataUpdate(0);
            }
        }
    }

    const auto viewModel = interfaces->entityList->getEntityFromHandle(local->viewModel());
    if (!viewModel)
        return;

    const auto viewModelWeapon = interfaces->entityList->getEntityFromHandle(viewModel->weapon());
    if (!viewModelWeapon)
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(viewModelWeapon->itemDefinitionIndex2());
    if (!def)
        return;

    viewModel->modelIndex() = interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel());

    const auto worldModel = interfaces->entityList->getEntityFromHandle(viewModelWeapon->weaponWorldModel());
    if (!worldModel)
        return;

    worldModel->modelIndex() = interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel());
}

static void applyWeapons(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localTeam = local->getTeamNumber();
    const auto localXuid = local->getSteamId();
    const auto itemSchema = memory->itemSystem()->getItemSchema();

    const auto highestEntityIndex = interfaces->entityList->getHighestEntityIndex();
    for (int i = memory->globalVars->maxClients + 1; i <= highestEntityIndex; ++i) {
        const auto entity = interfaces->entityList->getEntity(i);
        if (!entity || !entity->isWeapon())
            continue;

        const auto weapon = entity;
        if (weapon->originalOwnerXuid() != localXuid)
            continue;

        const auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (Helpers::isKnife(definitionIndex))
            continue;

        const auto def = itemSchema->getItemDefinitionInterface(definitionIndex);
        if (!def)
            continue;

        const auto loadoutSlot = def->getLoadoutSlot(localTeam);
        const auto itemView = localInventory.getItemInLoadout(localTeam, loadoutSlot);
        if (!itemView)
            continue;

        const auto soc = memory->getSOCData(itemView);
        if (!soc || !Inventory::getItem(soc->itemID))
            continue;

        weapon->accountID() = localInventory.getAccountID();
        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
    }
}

static void onPostDataUpdateStart(int localHandle) noexcept
{
    const auto local = interfaces->entityList->getEntityFromHandle(localHandle);
    if (!local)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

#ifndef _WIN32
    applyGloves(*localInventory, local);
#endif
    applyKnife(*localInventory, local);
    applyWeapons(*localInventory, local);
}

static bool hudUpdateRequired{ false };

static void updateHud() noexcept
{
    if (auto hud_weapons = memory->findHudElement(memory->hud, "CCSGO_HudWeaponSelection") - WIN32_LINUX(0x28, 62)) {
        for (int i = 0; i < *(hud_weapons + WIN32_LINUX(32, 52)); i++)
            i = memory->clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

void InventoryChanger::setToolToUse(std::uint64_t itemID) noexcept { ToolUser::setTool(itemID); }
void InventoryChanger::setItemToApplyTool(std::uint64_t itemID) noexcept { ToolUser::setDestItem(itemID, ToolUser::Action::Use); }
void InventoryChanger::setItemToWearSticker(std::uint64_t itemID) noexcept { ToolUser::setDestItem(itemID, ToolUser::Action::WearSticker); }
void InventoryChanger::setItemToRemoveNameTag(std::uint64_t itemID) noexcept { ToolUser::setDestItem(itemID, ToolUser::Action::RemoveNameTag);}
void InventoryChanger::setStatTrakSwapItem1(std::uint64_t itemID) noexcept { ToolUser::setStatTrakSwapItem1(itemID); }
void InventoryChanger::setStatTrakSwapItem2(std::uint64_t itemID) noexcept { ToolUser::setStatTrakSwapItem2(itemID); }
void InventoryChanger::setStickerApplySlot(int slot) noexcept { ToolUser::setStickerSlot(slot); }
void InventoryChanger::setStickerSlotToWear(int slot) noexcept { ToolUser::setStickerSlot(slot); }
void InventoryChanger::setNameTagString(const char* str) noexcept { ToolUser::setNameTag(str); }

void InventoryChanger::deleteItem(std::uint64_t itemID) noexcept
{
    if (const auto item = Inventory::getItem(itemID))
        item->markToDelete();
}

void InventoryChanger::acknowledgeItem(std::uint64_t itemID) noexcept
{
    if (Inventory::getItem(itemID) == nullptr)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
        if (const auto soc = memory->getSOCData(view)) {
            soc->inventory = localInventory->getHighestIDs().second;
            localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
        }
    }
}

static void applyMusicKit(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (pr == nullptr)
        return;

    const auto itemView = localInventory.getItemInLoadout(Team::None, 54);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isMusic())
        return;

    const auto& itemData = StaticData::paintKits()[item->get().dataIndex];
    pr->musicID()[localPlayer->index()] = itemData.id;
}

static void applyPlayerAgent(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto itemView = localInventory.getItemInLoadout(localPlayer->getTeamNumber(), 38);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isAgent())
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID);
    if (!def)
        return;

    const auto model = def->getPlayerDisplayModel();
    if (!model)
        return;

    const auto& dynamicData = Inventory::dynamicAgentData(item->getDynamicDataIndex());
    for (std::size_t i = 0; i < dynamicData.patches.size(); ++i) {
        if (const auto& patch = dynamicData.patches[i]; patch.patchID != 0)
            localPlayer->playerPatchIndices()[i] = patch.patchID;
    }

    const auto idx = interfaces->modelInfo->getModelIndex(model);
    localPlayer->setModelIndex(idx);

    if (const auto ragdoll = interfaces->entityList->getEntityFromHandle(localPlayer->ragdoll()))
        ragdoll->setModelIndex(idx);
}

static void applyMedal(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (!pr)
        return;

    const auto itemView = localInventory.getItemInLoadout(Team::None, 55);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isCollectible())
        return;

    pr->activeCoinRank()[localPlayer->index()] = static_cast<int>(item->get().weaponID);
}

void InventoryChanger::run(FrameStage stage) noexcept
{
    static int localPlayerHandle = -1;

    if (localPlayer)
        localPlayerHandle = localPlayer->handle();

    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        onPostDataUpdateStart(localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer->isDormant())
            updateHud();
    }

    if (stage != FrameStage::RENDER_START)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

#ifdef _WIN32
    if (localPlayer)
        applyGloves(*localInventory, localPlayer.get());
#endif;

    applyMusicKit(*localInventory);
    applyPlayerAgent(*localInventory);
    applyMedal(*localInventory);

    ToolUser::preAddItems(*localInventory);
    Inventory::runFrame();
}

void InventoryChanger::scheduleHudUpdate() noexcept
{
    interfaces->cvar->findVar("cl_fullupdate")->changeCallback();
    hudUpdateRequired = true;
}

void InventoryChanger::overrideHudIcon(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (event.getInt("attacker") != localPlayer->getUserId())
        return;

    if (const auto weapon = std::string_view{ event.getString("weapon") }; weapon != "knife" && weapon != "knife_t")
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || Inventory::getItem(soc->itemID) == nullptr)
        return;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(soc->weaponId)) {
        if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

void InventoryChanger::updateStatTrak(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    const auto itemID = weapon->itemID();
    const auto item = Inventory::getItem(itemID);
    if (!item || !item->isSkin())
        return;

    const auto itemView = memory->getInventoryItemByItemID(localInventory, itemID);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    auto& dynamicData = Inventory::dynamicSkinData(item->getDynamicDataIndex());
    if (dynamicData.statTrak > -1) {
        ++dynamicData.statTrak;
        soc->setStatTrak(dynamicData.statTrak);
        localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
    }
}

void InventoryChanger::onRoundMVP(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("userid") != localUserId)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(Team::None, 54);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isMusic())
        return;

    auto& dynamicData = Inventory::dynamicMusicData(item->getDynamicDataIndex());
    if (dynamicData.statTrak > -1) {
        ++dynamicData.statTrak;
        event.setInt("musickitmvps", dynamicData.statTrak);
        soc->setStatTrak(dynamicData.statTrak);
        localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
    }
}

static bool windowOpen = false;

void InventoryChanger::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Inventory Changer")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Inventory Changer");
        ImGui::SetWindowPos("Inventory Changer", { 100.0f, 100.0f });
    }
}

void InventoryChanger::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Inventory Changer")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

static ImTextureID getItemIconTexture(const std::string& iconpath) noexcept;

namespace ImGui
{
    static bool SkinSelectable(const StaticData::GameItem& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool selected, int* toAddCount = nullptr) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = StaticData::getWeaponName(item.weaponID).c_str();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = item.hasPaintKit() ? StaticData::paintKits()[item.dataIndex].name.c_str() : "";
        const auto paintKitNameSize = CalcTextSize(paintKitName, nullptr);

        PushID(itemName);
        PushID(paintKitName);
        const auto id = window->GetID(0);
        PopID();
        PopID();

        const auto height = ImMax(paintKitNameSize.y, ImMax(itemNameSize.y, iconSizeSmall.y));
        const auto rarityBulletRadius = IM_FLOOR(height * 0.2f);
        const auto size = ImVec2{ iconSizeSmall.x + rarityBulletRadius * 2.0f + itemNameSize.x + paintKitNameSize.x, height };
        
        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const auto smallIconMin = pos;
        const auto smallIconMax = smallIconMin + iconSizeSmall;

        const auto rarityBulletPos = ImVec2{ pos.x + iconSizeSmall.x + 5.0f + rarityBulletRadius, pos.y + IM_FLOOR(size.y * 0.5f) };

        const auto itemNameMin = ImVec2{ rarityBulletPos.x + rarityBulletRadius + 5.0f, pos.y };
        const auto itemNameMax = itemNameMin + ImVec2{ itemNameSize.x, size.y };
        
        const auto separatorHeightInv = IM_FLOOR(height * 0.2f);
        const auto separatorMin = ImVec2{ itemNameMax.x + 5.0f, pos.y + separatorHeightInv };
        const auto separatorMax = separatorMin + ImVec2{ 1.0f, height - 2.0f * separatorHeightInv };

        const auto paintKitNameMin = ImVec2{ separatorMax.x + 5.0f, pos.y };
        const auto paintKitNameMax = paintKitNameMin + ImVec2{ paintKitNameSize.x, size.y };

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos, pos + ImVec2{ ImMax(size.x, window->WorkRect.Max.x - pos.x), size.y });
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;
        const float spacingL = IM_FLOOR(spacingX * 0.50f);
        const float spacingU = IM_FLOOR(spacingY * 0.50f);
        bb.Min.x -= spacingL;
        bb.Min.y -= spacingU;
        bb.Max.x += (spacingX - spacingL);
        bb.Max.y += (spacingY - spacingU);

        if (!ItemAdd(bb, id))
            return false;

        const ImRect selectableBB{ bb.Min, ImVec2{ bb.Max.x - (selected ? 90.0f : 0.0f), bb.Max.y} };
        // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
        ImGuiButtonFlags buttonFlags = 0;
        bool hovered, held;
        bool pressed = ButtonBehavior(selectableBB, id, &hovered, &held, buttonFlags);

        // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
        if (pressed) {
            if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent) {
                SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImRect(bb.Min - window->Pos, bb.Max - window->Pos));
                g.NavDisableHighlight = true;
            }
            MarkItemEdited(id);
        }

        if (hovered || selected) {
            const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(item.iconPath)) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius, rarityColor, 12);

        RenderTextClipped(itemNameMin, itemNameMax, itemName, nullptr, &itemNameSize, { 0.0f, 0.5f }, &bb);
        if (paintKitName[0] != '\0')
            window->DrawList->AddRectFilled(separatorMin, separatorMax, GetColorU32(ImGuiCol_Text));
        RenderTextClipped(paintKitNameMin, paintKitNameMax, paintKitName, nullptr, &paintKitNameSize, { 0.0f, 0.5f }, &bb);

        if (selected && toAddCount) {
            const auto cursorPosNext = window->DC.CursorPos.y;
            SameLine(window->WorkRect.Max.x - pos.x - 90.0f);
            const auto cursorPosBackup = window->DC.CursorPos.y;

            window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
            SetNextItemWidth(80.0f);
            InputInt("", toAddCount);
            *toAddCount = (std::max)(*toAddCount, 1);

            window->DC.CursorPosPrevLine.y = cursorPosBackup;
            window->DC.CursorPos.y = cursorPosNext;
        }

        if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
            CloseCurrentPopup();

        return pressed;
    }

    static void SkinItem(const StaticData::GameItem& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool& shouldDelete) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = StaticData::getWeaponName(item.weaponID).c_str();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = item.hasPaintKit() ? StaticData::paintKits()[item.dataIndex].name.c_str() : "";
        const auto paintKitNameSize = CalcTextSize(paintKitName, nullptr);

        PushID(itemName);
        PushID(paintKitName);
        const auto id = window->GetID(0);
        PopID();
        PopID();

        const auto height = ImMax(paintKitNameSize.y, ImMax(itemNameSize.y, iconSizeSmall.y));
        const auto rarityBulletRadius = IM_FLOOR(height * 0.2f);
        const auto size = ImVec2{ iconSizeSmall.x + rarityBulletRadius * 2.0f + itemNameSize.x + paintKitNameSize.x, height };

        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const auto smallIconMin = pos;
        const auto smallIconMax = smallIconMin + iconSizeSmall;

        const auto rarityBulletPos = ImVec2{ pos.x + iconSizeSmall.x + 5.0f + rarityBulletRadius, pos.y + IM_FLOOR(size.y * 0.5f) };

        const auto itemNameMin = ImVec2{ rarityBulletPos.x + rarityBulletRadius + 5.0f, pos.y };
        const auto itemNameMax = itemNameMin + ImVec2{ itemNameSize.x, size.y };

        const auto separatorHeightInv = IM_FLOOR(height * 0.2f);
        const auto separatorMin = ImVec2{ itemNameMax.x + 5.0f, pos.y + separatorHeightInv };
        const auto separatorMax = separatorMin + ImVec2{ 1.0f, height - 2.0f * separatorHeightInv };

        const auto paintKitNameMin = ImVec2{ separatorMax.x + 5.0f, pos.y };
        const auto paintKitNameMax = paintKitNameMin + ImVec2{ paintKitNameSize.x, size.y };

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos, pos + ImVec2{ ImMax(size.x, window->WorkRect.Max.x - pos.x), size.y });
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;
        const float spacingL = IM_FLOOR(spacingX * 0.50f);
        const float spacingU = IM_FLOOR(spacingY * 0.50f);
        bb.Min.x -= spacingL;
        bb.Min.y -= spacingU;
        bb.Max.x += (spacingX - spacingL);
        bb.Max.y += (spacingY - spacingU);

        if (!ItemAdd(bb, id))
            return;

        if (const bool hovered = (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, bb.Max))) {
            const ImU32 col = GetColorU32(ImGuiCol_HeaderHovered);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(item.iconPath)) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius, rarityColor, 12);

        RenderTextClipped(itemNameMin, itemNameMax, itemName, nullptr, &itemNameSize, { 0.0f, 0.5f }, &bb);
        if (paintKitName[0] != '\0')
            window->DrawList->AddRectFilled(separatorMin, separatorMax, GetColorU32(ImGuiCol_Text));
        RenderTextClipped(paintKitNameMin, paintKitNameMax, paintKitName, nullptr, &paintKitNameSize, { 0.0f, 0.5f }, &bb);

        const auto removeButtonSize = CalcTextSize("Delete", nullptr) + style.FramePadding * 2.0f;
        const auto cursorPosNext = window->DC.CursorPos.y;
        SameLine(window->WorkRect.Max.x - pos.x - removeButtonSize.x - 7.0f);
        const auto cursorPosBackup = window->DC.CursorPos.y;

        window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
        if (Button("Delete"))
            shouldDelete = true;

        window->DC.CursorPosPrevLine.y = cursorPosBackup;
        window->DC.CursorPos.y = cursorPosNext;
    }
}

void InventoryChanger::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 400.0f });
        if (!ImGui::Begin("Inventory Changer", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            ImGui::End();
            return;
        }
    }

    static std::string filter;

    static bool isInAddMode = false;

    if (!isInAddMode && ImGui::Button("Add items.."))
        isInAddMode = true;

    if (!isInAddMode) {
        ImGui::SameLine();
        if (ImGui::Button("Force Update"))
            InventoryChanger::scheduleHudUpdate();
    }

    constexpr auto rarityColor = [](int rarity) noexcept {
        constexpr auto rarityColors = std::to_array<ImU32>({
            IM_COL32(0,   0,   0,   0),
            IM_COL32(176, 195, 217, 255),
            IM_COL32(94, 152, 217, 255),
            IM_COL32(75, 105, 255, 255),
            IM_COL32(136,  71, 255, 255),
            IM_COL32(211,  44, 230, 255),
            IM_COL32(235,  75,  75, 255),
            IM_COL32(228, 174,  57, 255)
            });
        return rarityColors[static_cast<std::size_t>(rarity) < rarityColors.size() ? rarity : 0];
    };

    if (isInAddMode) {
        static std::unordered_map<std::size_t, int> selectedToAdd;
        if (ImGui::Button("Back")) {
            isInAddMode = false;
            selectedToAdd.clear();
        }
        ImGui::SameLine();
        const auto canAdd = !selectedToAdd.empty();
        if (!canAdd) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::Button(("Add selected (" + std::to_string(selectedToAdd.size()) + ")").c_str())) {
            isInAddMode = false;
            addToInventory(selectedToAdd);
            selectedToAdd.clear();
        }
        if (!canAdd) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##search", "Search weapon skins, stickers, knives, gloves, music kits..", &filter);

        constexpr auto passesFilter = [](const std::wstring& str, std::wstring filter) {
            constexpr auto delimiter = L" ";
            wchar_t* _;
            wchar_t* token = std::wcstok(filter.data(), delimiter, &_);
            while (token) {
                if (!std::wcsstr(str.c_str(), token))
                    return false;
                token = std::wcstok(nullptr, delimiter, &_);
            }
            return true;
        };

        if (ImGui::BeginChild("##scrollarea", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            const auto& gameItems = StaticData::gameItems();
            const std::wstring filterWide = Helpers::toUpper(Helpers::toWideString(filter));
            for (std::size_t i = 0; i < gameItems.size(); ++i) {
                if (!filter.empty() && !passesFilter(StaticData::getWeaponNameUpper(gameItems[i].weaponID), filterWide) && (!gameItems[i].hasPaintKit() || !passesFilter(StaticData::paintKits()[gameItems[i].dataIndex].nameUpperCase, filterWide)))
                    continue;
                ImGui::PushID(i);

                const auto selected = selectedToAdd.contains(i);
                const auto toAddCount = selected ? &selectedToAdd[i] : nullptr;

                if (ImGui::SkinSelectable(gameItems[i], { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(gameItems[i].rarity), selected, toAddCount)) {
                    if (selected)
                        selectedToAdd.erase(i);
                    else
                        selectedToAdd.emplace(i, 1);
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    } else {
        if (ImGui::BeginChild("##scrollarea2", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            auto& inventory = Inventory::get();
            for (std::size_t i = inventory.size(); i-- > 0;) {
                if (inventory[i].isDeleted() || inventory[i].shouldDelete())
                    continue;

                ImGui::PushID(i);
                bool shouldDelete = false;
                ImGui::SkinItem(inventory[i].get(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(inventory[i].get().rarity), shouldDelete);
                if (shouldDelete)
                    inventory[i].markToDelete();
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }

    if (!contentOnly)
        ImGui::End();
}

constexpr auto CONFIG_VERSION = 2;

json InventoryChanger::toJson() noexcept
{
    json j;

    j["Version"] = CONFIG_VERSION;

    auto& items = j["Items"];
    for (const auto& item : Inventory::get()) {
        if (item.isDeleted())
            continue;

        json itemConfig;

        const auto& gameItem = item.get();
        switch (gameItem.type) {
        case StaticData::Type::Sticker: {
            itemConfig["Type"] = "Sticker";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Sticker ID"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::Glove: {
            itemConfig["Type"] = "Glove";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;

            const auto& dynamicData = Inventory::dynamicGloveData(item.getDynamicDataIndex());

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            break;
        }
        case StaticData::Type::Skin: {
            itemConfig["Type"] = "Skin";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;

            const auto& dynamicData = Inventory::dynamicSkinData(item.getDynamicDataIndex());

            if (dynamicData.isSouvenir)
                itemConfig["Is Souvenir"] = dynamicData.isSouvenir;
            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            if (dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;

            if (!dynamicData.nameTag.empty())
                itemConfig["Name Tag"] = dynamicData.nameTag;

            auto& stickers = itemConfig["Stickers"];
            for (std::size_t i = 0; i < dynamicData.stickers.size(); ++i) {
                const auto& sticker = dynamicData.stickers[i];
                if (sticker.stickerID == 0)
                    continue;

                json stickerConfig;
                stickerConfig["Sticker ID"] = sticker.stickerID;
                stickerConfig["Wear"] = sticker.wear;
                stickerConfig["Slot"] = i;
                stickers.push_back(std::move(stickerConfig));
            }
            break;
        }
        case StaticData::Type::Music: {
            itemConfig["Type"] = "Music";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Music ID"] = staticData.id;
            const auto& dynamicData = Inventory::dynamicMusicData(item.getDynamicDataIndex());
            if (dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;
            break;
        }
        case StaticData::Type::Collectible: {
            itemConfig["Type"] = "Collectible";
            const auto& staticData = StaticData::collectibles()[gameItem.dataIndex];
            itemConfig["Weapon ID"] = gameItem.weaponID;
            itemConfig["Is Original"] = staticData.isOriginal;
            break;
        }
        case StaticData::Type::NameTag: {
            itemConfig["Type"] = "Name Tag";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::Patch: {
            itemConfig["Type"] = "Patch";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Patch ID"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::Graffiti: {
            itemConfig["Type"] = "Graffiti";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Graffiti ID"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::SealedGraffiti: {
            itemConfig["Type"] = "Sealed Graffiti";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Graffiti ID"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::Agent: {
            itemConfig["Type"] = "Agent";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            
            const auto& dynamicData = Inventory::dynamicAgentData(item.getDynamicDataIndex());
            auto& stickers = itemConfig["Patches"];
            for (std::size_t i = 0; i < dynamicData.patches.size(); ++i) {
                const auto& patch = dynamicData.patches[i];
                if (patch.patchID == 0)
                    continue;

                json patchConfig;
                patchConfig["Patch ID"] = patch.patchID;
                patchConfig["Slot"] = i;
                stickers.push_back(std::move(patchConfig));
            }
            break;
        }
        case StaticData::Type::Case: {
            itemConfig["Type"] = "Case";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::CaseKey: {
            itemConfig["Type"] = "Case Key";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::OperationPass: {
            itemConfig["Type"] = "Operation Pass";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::StatTrakSwapTool: {
            itemConfig["Type"] = "StatTrak Swap Tool";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::ViewerPass: {
            itemConfig["Type"] = "Viewer Pass";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        }

        items.push_back(std::move(itemConfig));
    }

    if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
        auto& equipment = j["Equipment"];
        for (std::size_t i = 0; i < 57; ++i) {
            json slot;

            if (const auto itemCT = localInventory->getItemInLoadout(Team::CT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemCT); soc && Inventory::getItem(soc->itemID))
                    slot["CT"] = Inventory::getItemIndex(soc->itemID);
            }

            if (const auto itemTT = localInventory->getItemInLoadout(Team::TT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemTT); soc && Inventory::getItem(soc->itemID))
                    slot["TT"] = Inventory::getItemIndex(soc->itemID);
            }

            if (const auto itemNOTEAM = localInventory->getItemInLoadout(Team::None, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemNOTEAM); soc && Inventory::getItem(soc->itemID))
                    slot["NOTEAM"] = Inventory::getItemIndex(soc->itemID);
            }

            if (!slot.empty()) {
                slot["Slot"] = i;
                equipment.push_back(std::move(slot));
            }
        }
    }

    return j;
}

void InventoryChanger::fromJson(const json& j) noexcept
{
    if (!j.contains("Items"))
        return;

    const auto& items = j["Items"];
    if (!items.is_array())
        return;

    for (std::size_t i = 0; i < items.size(); ++i) {
        const auto& jsonItem = items[i];
        if (jsonItem.empty() || !jsonItem.is_object())
            continue;

        if (!jsonItem.contains("Type") || !jsonItem["Type"].is_string())
            continue;

        const std::string type = jsonItem["Type"];
        if (type == "Sticker") {
            if (!jsonItem.contains("Sticker ID") || !jsonItem["Sticker ID"].is_number_integer())
                continue;

            const int stickerID = jsonItem["Sticker ID"];
            if (const auto itemIndex = StaticData::getItemIndex(WeaponId::Sticker, stickerID); itemIndex != StaticData::InvalidItemIdx)
                Inventory::addItemAcknowledged(itemIndex, Inventory::INVALID_DYNAMIC_DATA_IDX);
        } else if (type == "Skin") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto itemIndex = StaticData::getItemIndex(weaponID, paintKit);
            if (itemIndex == StaticData::InvalidItemIdx)
                continue;

            DynamicSkinData dynamicData;

            if (jsonItem.contains("Is Souvenir")) {
                if (const auto& isSouvenir = jsonItem["Is Souvenir"]; isSouvenir.is_boolean())
                    dynamicData.isSouvenir = isSouvenir;
            }

            if (jsonItem.contains("Wear")) {
                if (const auto& wear = jsonItem["Wear"]; wear.is_number_float())
                    dynamicData.wear = wear;
            }

            if (jsonItem.contains("Seed")) {
                if (const auto& seed = jsonItem["Seed"]; seed.is_number_integer())
                    dynamicData.seed = seed;
            }

            if (jsonItem.contains("StatTrak")) {
                if (const auto& statTrak = jsonItem["StatTrak"]; statTrak.is_number_integer())
                    dynamicData.statTrak = statTrak;
            }

            if (jsonItem.contains("Name Tag")) {
                if (const auto& nameTag = jsonItem["Name Tag"]; nameTag.is_string())
                    dynamicData.nameTag = nameTag;
            }

            if (jsonItem.contains("Stickers")) {
                if (const auto& stickers = jsonItem["Stickers"]; stickers.is_array()) {
                    for (std::size_t k = 0; k < stickers.size(); ++k) {
                        const auto& sticker = stickers[k];
                        if (!sticker.is_object())
                            continue;

                        if (!sticker.contains("Sticker ID") || !sticker["Sticker ID"].is_number_integer())
                            continue;

                        if (!sticker.contains("Slot") || !sticker["Slot"].is_number_integer())
                            continue;

                        const int stickerID = sticker["Sticker ID"];
                        if (stickerID == 0)
                            continue;
                        const std::size_t slot = sticker["Slot"];
                        if (slot >= std::tuple_size_v<decltype(DynamicSkinData::stickers)>)
                            continue;
                        dynamicData.stickers[slot].stickerID = stickerID;
                        if (sticker.contains("Wear") && sticker["Wear"].is_number_float())
                            dynamicData.stickers[slot].wear = sticker["Wear"];
                    }
                }
            }

            Inventory::addItemAcknowledged(itemIndex, Inventory::emplaceDynamicData(std::move(dynamicData)));
        } else if (type == "Glove") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto itemIndex = StaticData::getItemIndex(weaponID, paintKit);
            if (itemIndex == StaticData::InvalidItemIdx)
                continue;

            DynamicGloveData dynamicData;

            if (jsonItem.contains("Wear")) {
                if (const auto& wear = jsonItem["Wear"]; wear.is_number_float())
                    dynamicData.wear = wear;
            }

            if (jsonItem.contains("Seed")) {
                if (const auto& seed = jsonItem["Seed"]; seed.is_number_integer())
                    dynamicData.seed = seed;
            }

            Inventory::addItemAcknowledged(itemIndex, Inventory::emplaceDynamicData(std::move(dynamicData)));
        } else if (type == "Music") {
            if (!jsonItem.contains("Music ID") || !jsonItem["Music ID"].is_number_integer())
                continue;

            const int musicID = jsonItem["Music ID"];

            const auto itemIndex = StaticData::getItemIndex(WeaponId::MusicKit, musicID);
            if (itemIndex == StaticData::InvalidItemIdx)
                continue;

            DynamicMusicData dynamicData;

            if (jsonItem.contains("StatTrak")) {
                if (const auto& statTrak = jsonItem["StatTrak"]; statTrak.is_number_integer() && statTrak > -1)
                    dynamicData.statTrak = statTrak;
            }

            Inventory::addItemAcknowledged(itemIndex, Inventory::emplaceDynamicData(std::move(dynamicData)));
        } else if (type == "Collectible") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;
            if (!jsonItem.contains("Is Original") || !jsonItem["Is Original"].is_boolean())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];
            const bool isOriginal = jsonItem["Is Original"];

            // FIXME: not using StaticData::getItemIndex() because it doesn't support original collectibles
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID, isOriginal](const auto& gameItem) { return gameItem.isCollectible() && gameItem.weaponID == weaponID && StaticData::collectibles()[gameItem.dataIndex].isOriginal == isOriginal; });

            if (staticData == StaticData::gameItems().end())
                continue;

            Inventory::addItemAcknowledged(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX);
        } else if (type == "Name Tag") {
            if (const auto itemIndex = StaticData::getItemIndex(WeaponId::NameTag, 0); itemIndex != StaticData::InvalidItemIdx)
                Inventory::addItemAcknowledged(itemIndex, Inventory::INVALID_DYNAMIC_DATA_IDX);
        } else if (type == "Patch") {
            if (!jsonItem.contains("Patch ID") || !jsonItem["Patch ID"].is_number_integer())
                continue;

            const int patchID = jsonItem["Patch ID"];

            if (const auto itemIndex = StaticData::getItemIndex(WeaponId::Patch, patchID); itemIndex != StaticData::InvalidItemIdx)
                Inventory::addItemAcknowledged(itemIndex, Inventory::INVALID_DYNAMIC_DATA_IDX);
        } else if (type == "Graffiti") {
            if (!jsonItem.contains("Graffiti ID") || !jsonItem["Graffiti ID"].is_number_integer())
                continue;

            const int graffitiID = jsonItem["Graffiti ID"];

            if (const auto itemIndex = StaticData::getItemIndex(WeaponId::Graffiti, graffitiID); itemIndex != StaticData::InvalidItemIdx)
                Inventory::addItemAcknowledged(itemIndex, Inventory::INVALID_DYNAMIC_DATA_IDX);
        } else if (type == "Sealed Graffiti") {
            if (!jsonItem.contains("Graffiti ID") || !jsonItem["Graffiti ID"].is_number_integer())
                continue;

            const int graffitiID = jsonItem["Graffiti ID"];

            if (const auto itemIndex = StaticData::getItemIndex(WeaponId::SealedGraffiti, graffitiID); itemIndex != StaticData::InvalidItemIdx)
                Inventory::addItemAcknowledged(itemIndex, Inventory::INVALID_DYNAMIC_DATA_IDX);
        } else if (type == "Agent") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;
            
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto itemIndex = StaticData::getItemIndex(weaponID, 0);
            if (itemIndex == StaticData::InvalidItemIdx)
                continue;

            DynamicAgentData dynamicData;

            if (jsonItem.contains("Patches")) {
                if (const auto& patches = jsonItem["Patches"]; patches.is_array()) {
                    for (std::size_t k = 0; k < patches.size(); ++k) {
                        const auto& patch = patches[k];
                        if (!patch.is_object())
                            continue;

                        if (!patch.contains("Patch ID") || !patch["Patch ID"].is_number_integer())
                            continue;

                        if (!patch.contains("Slot") || !patch["Slot"].is_number_integer())
                            continue;

                        const int patchID = patch["Patch ID"];
                        if (patchID == 0)
                            continue;
                        const std::size_t slot = patch["Slot"];
                        if (slot >= std::tuple_size_v<decltype(DynamicAgentData::patches)>)
                            continue;
                        dynamicData.patches[slot].patchID = patchID;
                    }
                }
            }

            Inventory::addItemAcknowledged(itemIndex, Inventory::emplaceDynamicData(std::move(dynamicData)));
        } else if (type == "Case" || type == "Case Key" || type == "Operation Pass" || type == "StatTrak Swap Tool" || type == "Viewer Pass") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];

            if (const auto itemIndex = StaticData::getItemIndex(weaponID, 0); itemIndex != StaticData::InvalidItemIdx)
                Inventory::addItemAcknowledged(itemIndex, Inventory::INVALID_DYNAMIC_DATA_IDX);
        }
    }

    if (!j.contains("Equipment"))
        return;

    const auto& equipment = j["Equipment"];
    if (!equipment.is_array())
        return;

    for (std::size_t i = 0; i < equipment.size(); ++i) {
        const auto& equipped = equipment[i];
        if (!equipped.contains("Slot"))
            continue;

        const auto& slot = equipped["Slot"];
        if (!slot.is_number_integer())
            continue;

        if (equipped.contains("CT")) {
            if (const auto& ct = equipped["CT"]; ct.is_number_integer())
                Inventory::equipItem(Team::CT, slot, ct);
        }

        if (equipped.contains("TT")) {
            if (const auto& tt = equipped["TT"]; tt.is_number_integer())
                Inventory::equipItem(Team::TT, slot, tt);
        }
         
        if (equipped.contains("NOTEAM")) {
            if (const auto& noteam = equipped["NOTEAM"]; noteam.is_number_integer())
                Inventory::equipItem(Team::None, slot, noteam);
        }
    }
}

void InventoryChanger::resetConfig() noexcept
{
    Inventory::clear();
}

void InventoryChanger::clearInventory() noexcept
{
    resetConfig();
}

static std::size_t lastEquippedCount = 0;
void InventoryChanger::onItemEquip(Team team, int slot, std::uint64_t itemID) noexcept
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto item = Inventory::getItem(itemID);
    if (!item)
        return;

    if (item->isCollectible()) {
        if (const auto view = memory->getInventoryItemByItemID(localInventory, itemID)) {
            if (const auto econItem = memory->getSOCData(view))
                localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
        }
    } else if (item->isSkin()) {
        const auto view = localInventory->getItemInLoadout(team, slot);
        memory->inventoryManager->equipItemInSlot(team, slot, (std::uint64_t(0xF) << 60) | static_cast<short>(item->get().weaponID));
        if (view) {
            if (const auto econItem = memory->getSOCData(view))
                localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
        }
        ++lastEquippedCount;
    }
}

void InventoryChanger::onSoUpdated(SharedObject* object) noexcept
{
    if (lastEquippedCount > 0 && object->getTypeID() == 43 /* = k_EEconTypeDefaultEquippedDefinitionInstanceClient */) {
        *reinterpret_cast<WeaponId*>(std::uintptr_t(object) + WIN32_LINUX(0x10, 0x1C)) = WeaponId::None;
        --lastEquippedCount;
    }
}

void InventoryChanger::onUserTextMsg(const void*& data, int& size) noexcept
{
    if (!localPlayer)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    if (const auto item = Inventory::getItem(soc->itemID); !item || !item->isSkin())
        return;

    // https://github.com/SteamDatabase/Protobufs/blob/017f1710737b7026cdd6d7e602f96a66dddb7b2e/csgo/cstrike15_usermessages.proto#L128-L131

    const auto reader = ProtobufReader{ static_cast<const std::uint8_t*>(data), size };
    if (reader.readInt32(1) != 4 /* HUD_PRINTCENTER */)
        return;

    const auto strings = reader.readRepeatedString(3);
    if (strings.size() < 2)
        return;

    if (strings[0] != "#SFUI_Notice_CannotDropWeapon" && strings[0] != "#SFUI_Notice_YouDroppedWeapon")
        return;

    if (strings[1] != "#SFUI_WPNHUD_Knife" && strings[1] != "#SFUI_WPNHUD_Knife_T")
        return;

    const auto itemSchema = memory->itemSystem()->getItemSchema();
    if (!itemSchema)
        return;

    const auto def = itemSchema->getItemDefinitionInterface(soc->weaponId);
    if (!def)
        return;

    const auto itemBaseName = std::string_view{ def->getItemBaseName() };

    static std::vector<char> buffer;
    buffer = std::vector<char>{ 0x8, 0x4 /* HUD_PRINTCENTER */, 0x1A, static_cast<char>(strings[0].length()) };
    std::ranges::copy(strings[0], std::back_inserter(buffer));
    buffer.push_back(0x1A);
    buffer.push_back(static_cast<char>(itemBaseName.length()));
    std::ranges::copy(itemBaseName, std::back_inserter(buffer));

    // Add three empty strings, like UTIL_ClientPrintFilter() does
    constexpr auto emptyStrings = std::to_array<char>({ 0x1A, 0, 0x1A, 0, 0x1A, 0 });
    std::ranges::copy(emptyStrings, std::back_inserter(buffer));

    data = buffer.data();
    size = static_cast<int>(buffer.size());
}

struct Icon {
    Texture texture;
    int lastReferencedFrame = 0;
};

static std::unordered_map<std::string, Icon> iconTextures;

static ImTextureID getItemIconTexture(const std::string& iconpath) noexcept
{
    if (iconpath.empty())
        return 0;

    auto& icon = iconTextures[iconpath];
    if (!icon.texture.get()) {
        static int frameCount = 0;
        static float timeSpentThisFrame = 0.0f;
        static int loadedThisFrame = 0;

        if (frameCount != ImGui::GetFrameCount()) {
            frameCount = ImGui::GetFrameCount();
            timeSpentThisFrame = 0.0f;
            // memory->debugMsg("LOADED %d ICONS\n", loadedThisFrame);
            loadedThisFrame = 0;
        }

        if (timeSpentThisFrame > 0.01f)
            return 0;

        ++loadedThisFrame;

        const auto start = std::chrono::high_resolution_clock::now();

        auto handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + (iconpath.find("status_icons") != std::string::npos ? "" : "_large") + ".png").c_str(), "r", "GAME");
        if (!handle)
            handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + ".png").c_str(), "r", "GAME");

        assert(handle);
        if (handle) {
            if (const auto size = interfaces->baseFileSystem->size(handle); size > 0) {
                const auto buffer = std::make_unique<std::uint8_t[]>(size);
                if (interfaces->baseFileSystem->read(buffer.get(), size, handle) > 0) {
                    int width, height;
                    stbi_set_flip_vertically_on_load_thread(false);

                    if (const auto data = stbi_load_from_memory((const stbi_uc*)buffer.get(), size, &width, &height, nullptr, STBI_rgb_alpha)) {
                        icon.texture.init(width, height, data);
                        stbi_image_free(data);
                    } else {
                        assert(false);
                    }
                }
            }
            interfaces->baseFileSystem->close(handle);
        }

        const auto end = std::chrono::high_resolution_clock::now();
        timeSpentThisFrame += std::chrono::duration<float>(end - start).count();
    }
    icon.lastReferencedFrame = ImGui::GetFrameCount();
    return icon.texture.get();
}

void InventoryChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}

void InventoryChanger::clearUnusedItemIconTextures() noexcept
{
    constexpr auto maxIcons = 30;
    const auto frameCount = ImGui::GetFrameCount();
    while (iconTextures.size() > maxIcons) {
        const auto oldestIcon = std::ranges::min_element(iconTextures, [](const auto& a, const auto& b) { return a.second.lastReferencedFrame < b.second.lastReferencedFrame; });
        if (oldestIcon->second.lastReferencedFrame == frameCount)
            break;

        iconTextures.erase(oldestIcon);
    }
}

static int remapKnifeAnim(WeaponId weaponID, const int sequence) noexcept
{
    enum Sequence
    {
        SEQUENCE_DEFAULT_DRAW = 0,
        SEQUENCE_DEFAULT_IDLE1 = 1,
        SEQUENCE_DEFAULT_IDLE2 = 2,
        SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
        SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
        SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
        SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
        SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
        SEQUENCE_DEFAULT_LOOKAT01 = 12,

        SEQUENCE_BUTTERFLY_DRAW = 0,
        SEQUENCE_BUTTERFLY_DRAW2 = 1,
        SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
        SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

        SEQUENCE_FALCHION_IDLE1 = 1,
        SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
        SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
        SEQUENCE_FALCHION_LOOKAT01 = 12,
        SEQUENCE_FALCHION_LOOKAT02 = 13,

        SEQUENCE_DAGGERS_IDLE1 = 1,
        SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
        SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
        SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
        SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

        SEQUENCE_BOWIE_IDLE1 = 1,
    };

    switch (weaponID) {
    case WeaponId::Butterfly:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
            return Helpers::random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
        default:
            return sequence + 1;
        }
    case WeaponId::Falchion:
        switch (sequence) {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_FALCHION_IDLE1;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return Helpers::random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence - 1;
        }
    case WeaponId::Daggers:
        switch (sequence) {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return Helpers::random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return Helpers::random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
        case SEQUENCE_DEFAULT_LOOKAT01:
            return sequence + 3;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence + 2;
        }
    case WeaponId::Bowie:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_BOWIE_IDLE1;
        default:
            return sequence - 1;
        }
    case WeaponId::Ursus:
    case WeaponId::SkeletonKnife:
    case WeaponId::NomadKnife:
    case WeaponId::Paracord:
    case WeaponId::SurvivalKnife:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
            return Helpers::random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
        default:
            return sequence + 1;
        }
    case WeaponId::Stiletto:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(12, 13);
        }
    case WeaponId::Talon:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return Helpers::random(14, 15);
        }
    default:
        return sequence;
    }
}

void InventoryChanger::fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept
{
    if (!localPlayer)
        return;

    if (!Helpers::isKnife(viewModelWeapon->itemDefinitionIndex2()))
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
    if (!itemView)
        return;

    if (const auto soc = memory->getSOCData(itemView); !soc || Inventory::getItem(soc->itemID) == nullptr)
        return;

    sequence = remapKnifeAnim(viewModelWeapon->itemDefinitionIndex2(), sequence);
}
