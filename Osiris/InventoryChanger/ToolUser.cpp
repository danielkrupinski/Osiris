#include "../Interfaces.h"
#include "Inventory.h"
#include "ItemGenerator.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Panorama.h"
#include "StaticData.h"
#include "ToolUser.h"

#include "GameItems/Lookup.h"

static void initItemCustomizationNotification(std::string_view typeStr, std::uint64_t itemID) noexcept
{
    const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_Inventory_ItemCustomizationNotification"));
    if (idx == -1)
        return;

    using namespace std::string_view_literals;
    std::string args{ "0,'" }; args += typeStr; args += "','"sv; args += std::to_string(itemID); args += '\'';
    const char* dummy;
    if (const auto event = memory->registeredPanoramaEvents->memory[idx].value.createEventFromString(nullptr, args.c_str(), &dummy))
        interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(event);
}

enum class Action {
    Use,
    WearSticker,
    RemoveNameTag
};

class ToolUserImpl {
public:
    static void setDestItem(std::uint64_t itemID, Action action) noexcept
    {
        instance().destItemID = itemID;
        instance().action = action;
        instance().useTime = memory->globalVars->realtime + Helpers::random(0.18f, 0.48f);
    }

    static void setTool(std::uint64_t itemID) noexcept { instance().toolItemID = itemID; }
    static void setNameTag(const char* nameTag) noexcept { instance().nameTag = nameTag; }
    static void setStickerSlot(int slot) noexcept { instance().stickerSlot = slot; }
    static void setStatTrakSwapItem1(std::uint64_t itemID) noexcept { instance().statTrakSwapItem1 = itemID; }
    static void setStatTrakSwapItem2(std::uint64_t itemID) noexcept { instance().statTrakSwapItem2 = itemID; }

    static void preAddItems(CSPlayerInventory& localInventory) noexcept { instance()._preAddItems(localInventory); }
private:
    void _wearSticker(CSPlayerInventory& localInventory) const noexcept
    {
        const auto dest = Inventory::getItem(destItemID);
        if (!dest)
            return;

        if (dest->isSkin()) {
            constexpr auto wearStep = 0.12f;
            const auto newWear = (Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot].wear += wearStep);

            if (const auto shouldRemove = (newWear >= 1.0f + wearStep); shouldRemove) {
                Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot] = {};
                initItemCustomizationNotification("sticker_remove", Inventory::recreateItem(destItemID));
            } else {
                if (const auto view = memory->findOrCreateEconItemViewForItemID(destItemID)) {
                    if (const auto soc = memory->getSOCData(view)) {
                        soc->setStickerWear(stickerSlot, newWear);
                        localInventory.soUpdated(localInventory.getSOID(), (SharedObject*)soc, 4);
                    }
                }
            }
        } else if (dest->isAgent()) {
            Inventory::dynamicAgentData(dest->getDynamicDataIndex()).patches[stickerSlot] = {};
            initItemCustomizationNotification("patch_remove", Inventory::recreateItem(destItemID));
        }
    }

    void _removeNameTag() const noexcept
    {
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).nameTag.clear();
        Inventory::recreateItem(destItemID);
    }

    void _activateOperationPass(InventoryItem& pass) const noexcept
    {
        const auto passWeaponID = pass.get().getWeaponID();
        pass.markToDelete();
        const auto coinID = passWeaponID != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(passWeaponID) + 1) : WeaponId::BronzeOperationHydraCoin;
        if (const auto item = StaticData::lookup().findItem(coinID); item.has_value())
            Inventory::addItemNow(*item, Inventory::InvalidDynamicDataIdx, true);
    }

    void _activateViewerPass(InventoryItem& pass) const noexcept
    {
        const auto coinID = static_cast<WeaponId>(static_cast<int>(pass.get().getWeaponID()) + 1);
        pass.markToDelete();
        if (const auto item = StaticData::lookup().findItem(coinID); item.has_value())
            initItemCustomizationNotification("ticket_activated", Inventory::addItemNow(*item, Inventory::InvalidDynamicDataIdx, false));
    }

    void _unsealGraffiti(InventoryItem& sealedGraffiti) const noexcept
    {
        if (const auto item = StaticData::lookup().findGraffiti(StaticData::getSealedGraffitiID(sealedGraffiti.get())); item.has_value()) {
            sealedGraffiti.markToDelete();
            initItemCustomizationNotification("graffity_unseal", Inventory::addItemNow(*item, Inventory::InvalidDynamicDataIdx, false));
        }
    }

    void _openContainer(InventoryItem& container) const noexcept
    {
        assert(container.isCase());
        const auto& caseData = StaticData::getCase(container.get());
        assert(caseData.hasLoot());
        if (caseData.hasLoot()) {
            const auto [unlockedItem, dynamicDataIdx] = ItemGenerator::generateItemFromContainer(container);
            container.markToDelete();
            if (const auto tool = Inventory::getItem(toolItemID); tool && tool->isCaseKey())
                tool->markToDelete();
            initItemCustomizationNotification("crate_unlock", Inventory::addItemNow(unlockedItem, dynamicDataIdx, true));
        }
    }

    void _applySticker(InventoryItem& sticker) const noexcept
    {
        assert(sticker.isSticker());
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot].stickerID = StaticData::getStickerID(sticker.get());
        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).stickers[stickerSlot].wear = 0.0f;
        sticker.markToDelete();
        initItemCustomizationNotification("sticker_apply", Inventory::recreateItem(destItemID));
    }

    void _addNameTag(InventoryItem& nameTagItem) const noexcept
    {
        assert(nameTagItem.isNameTag());
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        Inventory::dynamicSkinData(dest->getDynamicDataIndex()).nameTag = nameTag;
        nameTagItem.markToDelete();
        initItemCustomizationNotification("nametag_add", Inventory::recreateItem(destItemID));
    }

    void _applyPatch(InventoryItem& patch) const noexcept
    {
        assert(patch.isPatch());
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isAgent())
            return;

        Inventory::dynamicAgentData(dest->getDynamicDataIndex()).patches[stickerSlot].patchID = StaticData::getPatchID(patch.get());
        patch.markToDelete();
        initItemCustomizationNotification("patch_apply", Inventory::recreateItem(destItemID));
    }

    void _swapStatTrak(InventoryItem& statTrakSwapTool) const noexcept
    {
        const auto item1 = Inventory::getItem(statTrakSwapItem1);
        if (!item1 || !item1->isSkin())
            return;

        const auto item2 = Inventory::getItem(statTrakSwapItem2);
        if (!item2 || !item2->isSkin())
            return;

        std::swap(Inventory::dynamicSkinData(item1->getDynamicDataIndex()).statTrak, Inventory::dynamicSkinData(item2->getDynamicDataIndex()).statTrak);
        statTrakSwapTool.markToDelete();

        const auto recreatedItemID1 = Inventory::recreateItem(statTrakSwapItem1);
        const auto recreatedItemID2 = Inventory::recreateItem(statTrakSwapItem2);
        if (const auto inventoryComponent = *memory->uiComponentInventory) {
            memory->setItemSessionPropertyValue(inventoryComponent, recreatedItemID1, "updated", "1");
            memory->setItemSessionPropertyValue(inventoryComponent, recreatedItemID2, "updated", "1");
        }
        initItemCustomizationNotification("stattrack_swap", recreatedItemID2);
    }

    void _activateSouvenirToken(InventoryItem& souvenirToken, CSPlayerInventory& localInventory) const noexcept
    {
        assert(souvenirToken.isSouvenirToken());

        const auto& inventory = Inventory::get();
        const auto it = std::ranges::find_if(inventory, [&souvenirToken](const auto& inventoryItem) { return inventoryItem.isTournamentCoin() && StaticData::getTournamentEventID(inventoryItem.get()) == StaticData::getTournamentEventID(souvenirToken.get()); });
        if (it != inventory.cend()) {
            souvenirToken.markToDelete();

            const auto newDropsAwarded = (++Inventory::dynamicTournamentCoinData(it->getDynamicDataIndex()).dropsAwarded);
            const auto coinItemID = std::distance(inventory.begin(), it) + Inventory::BASE_ITEMID;

            if (const auto view = memory->findOrCreateEconItemViewForItemID(coinItemID)) {
                if (const auto soc = memory->getSOCData(view)) {
                    soc->setDropsAwarded(newDropsAwarded);
                    localInventory.soUpdated(localInventory.getSOID(), (SharedObject*)soc, 4);
                }
            }

            initItemCustomizationNotification("ticket_activated", coinItemID);
        }
    }

    void _useTool(CSPlayerInventory& localInventory) const noexcept
    {
        if (const auto destItem = Inventory::getItem(destItemID); destItem && destItem->isCase()) {
            _openContainer(*destItem);
            return;
        }

        const auto tool = Inventory::getItem(toolItemID);
        if (!tool)
            return;

        if (tool->isSealedGraffiti()) {
            _unsealGraffiti(*tool);
        } else if (tool->isOperationPass()) {
            _activateOperationPass(*tool);
        } else if (tool->isSticker()) {
            _applySticker(*tool);
        } else if (tool->isNameTag()) {
            _addNameTag(*tool);
        } else if (tool->isPatch()) {
            _applyPatch(*tool);
        } else if (tool->isStatTrakSwapTool()) {
            _swapStatTrak(*tool);
        } else if (tool->isViewerPass()) {
            _activateViewerPass(*tool);
        } else if (tool->isSouvenirToken()) {
            _activateSouvenirToken(*tool, localInventory);
        }
    }

    void _preAddItems(CSPlayerInventory& localInventory) noexcept
    {
        if (useTime > memory->globalVars->realtime)
            return;

        if (action == Action::WearSticker) {
            _wearSticker(localInventory);
        } else if (action == Action::RemoveNameTag) {
            _removeNameTag();
        } else if (action == Action::Use) {
            _useTool(localInventory);
        }

        toolItemID = destItemID = statTrakSwapItem1 = statTrakSwapItem2 = 0;
    }

    static ToolUserImpl& instance() noexcept
    {
        static ToolUserImpl toolUser;
        return toolUser;
    }

    std::uint64_t toolItemID = 0;
    std::uint64_t destItemID = 0;
    std::uint64_t statTrakSwapItem1 = 0;
    std::uint64_t statTrakSwapItem2 = 0;
    Action action;
    float useTime = 0.0f;
    int stickerSlot = 0;
    std::string nameTag;
};

void ToolUser::setTool(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setTool(itemID);
}

void ToolUser::setItemToApplyTool(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setDestItem(itemID, Action::Use);
}

void ToolUser::setItemToWearSticker(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setDestItem(itemID, Action::WearSticker);
}

void ToolUser::setItemToRemoveNameTag(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setDestItem(itemID, Action::RemoveNameTag);
}

void ToolUser::setNameTag(const char* nameTag) noexcept
{
    ToolUserImpl::setNameTag(nameTag);
}

void ToolUser::setStickerSlot(int slot) noexcept
{
    ToolUserImpl::setStickerSlot(slot);
}

void ToolUser::setStatTrakSwapItem1(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setStatTrakSwapItem1(itemID);
}

void ToolUser::setStatTrakSwapItem2(std::uint64_t itemID) noexcept
{
    ToolUserImpl::setStatTrakSwapItem2(itemID);
}

void ToolUser::preAddItems(CSPlayerInventory& localInventory) noexcept
{
    ToolUserImpl::preAddItems(localInventory);
}
