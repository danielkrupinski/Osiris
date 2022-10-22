#pragma once

#include <optional>

#include "Item.h"

namespace inventory_changer::backend
{

class XRayScanner {
public:
    struct Items {
        ItemIterator reward;
        ItemIterator crate;
    };

    void storeItems(Items newItems)
    {
        items = newItems;
    }

    void onItemRemoval(ItemIterator item) noexcept
    {
        if (items.has_value() && (item == items->reward || item == items->crate))
            items.reset();
    }

    [[nodiscard]] std::optional<Items> getItems() const noexcept
    {
        return items;
    }

private:
    std::optional<Items> items;
};

}
