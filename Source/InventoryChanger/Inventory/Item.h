#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <variant>

#include <InventoryChanger/GameItems/Item.h>

namespace inventory
{

struct Item {
private:
    std::optional<std::reference_wrapper<const game_items::Item>> item;
    std::size_t dynamicDataIndex = static_cast<std::size_t>(-1);
    bool toDelete = false;
public:
    explicit Item(const game_items::Item& item, std::size_t dynamicDataIndex) noexcept : item{ item }, dynamicDataIndex{ dynamicDataIndex } {}

    void markAsDeleted() noexcept { item.reset(); }
    bool isDeleted() const noexcept { return !item.has_value(); }
    void markToDelete() noexcept { toDelete = true; }
    bool shouldDelete() const noexcept { return toDelete; }
    bool isValid() const noexcept { return !isDeleted() && !shouldDelete(); }

    bool isSticker() const noexcept { return isValid() && get().isSticker(); }
    bool isSkin() const noexcept { return isValid() && get().isSkin(); }
    bool isGlove() const noexcept { return isValid() && get().isGloves(); }
    bool isMusic() const noexcept { return isValid() && get().isMusic(); }
    bool isAgent() const noexcept { return isValid() && get().isAgent(); }
    bool isCollectible() const noexcept { return isValid() && get().isCollectible(); }
    bool isCase() const noexcept { return isValid() && get().isCase(); }
    bool isCaseKey() const noexcept { return isValid() && get().isCaseKey(); }
    bool isGraffiti() const noexcept { return isValid() && get().isGraffiti(); }
    bool isOperationPass() const noexcept { return isValid() && get().isOperationPass(); }
    bool isNameTag() const noexcept { return isValid() && get().isNameTag(); }
    bool isPatch() const noexcept { return isValid() && get().isPatch(); }
    bool isStatTrakSwapTool() const noexcept { return isValid() && get().isStatTrakSwapTool(); }
    bool isViewerPass() const noexcept { return isValid() && get().isViewerPass(); }
    bool isServiceMedal() const noexcept { return isValid() && get().isServiceMedal(); }
    bool isSouvenirToken() const noexcept { return isValid() && get().isSouvenirToken(); }
    bool isTournamentCoin() const noexcept { return isValid() && get().isTournamentCoin(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const game_items::Item& get() const noexcept { assert(isValid()); return item.value(); }
};

class StructWrapper {
public:
    template <typename T>
    StructWrapper(const T& object) : variant{ ValueWrapper{ object } } {}

    template <typename T>
    [[nodiscard]] T* get()
    {
        if (const auto item = std::get_if<ValueWrapper<T>>(&variant))
            return &item->get();
        return nullptr;
    }

    template <typename T>
    [[nodiscard]] const T* get() const
    {
        if (const auto item = std::get_if<ValueWrapper<T>>(&variant))
            return &item->get();
        return nullptr;
    }

private:
    template <typename T, typename = void>
    struct ValueWrapper {
        explicit ValueWrapper(const T& t) : object{ std::make_unique<T>(t) } {}

        T& get() noexcept { return *object.get(); }
        const T& get() const noexcept { return *object.get(); }
    private:
        std::unique_ptr<T> object;
    };

    template <typename T>
    struct AlwaysFalse : std::false_type {};

    // yet to be enabled for some types
    template <typename T>
    struct ValueWrapper<T, std::enable_if_t<AlwaysFalse<T>::value>> {
        explicit ValueWrapper(const T& t) : object{ t } { }

        T& get() noexcept { return object; }
        const T& get() const noexcept { return object; }
    private:
        T object;
    };

    std::variant<
        ValueWrapper<Skin>,
        ValueWrapper<Glove>,
        ValueWrapper<Agent>,
        ValueWrapper<Music>,
        ValueWrapper<Graffiti>,
        ValueWrapper<ServiceMedal>,
        ValueWrapper<SouvenirPackage>
    > variant;
};

class Item_v2 {
public:
    explicit Item_v2(const game_items::Item& item, StructWrapper data) noexcept : item{ item }, data{ std::move(data) } {}

    [[nodiscard]] const game_items::Item& gameItem() const noexcept { return item; }
    [[nodiscard]] StructWrapper& getData() noexcept { return data; }
    [[nodiscard]] const StructWrapper& getData() const noexcept { return data; }

private:
    std::reference_wrapper<const game_items::Item> item;
    StructWrapper data;
};

}
