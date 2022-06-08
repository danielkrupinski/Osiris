#pragma once

#include <cassert>
#include <functional>
#include <span>
#include <utility>
#include <vector>

#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

template <typename ItemType>
class SortFilter {
public:
    explicit SortFilter(std::span<const ItemType> items)
        : items{ items.begin(), items.end() }, passesFilter(items.size(), true) {}

    template <typename Predicate>
    void sort(Predicate&& predicate)
    {
        ranges::sort(ranges::views::zip(items, passesFilter), std::forward<Predicate>(predicate), [](const auto& pair) -> decltype(auto) { return std::get<0>(pair).get(); });
    }

    template <typename Predicate>
    void filter(Predicate predicate)
    {
        assert(items.size() == passesFilter.size());

        for (std::size_t i = 0; i < items.size(); ++i)
            passesFilter[i] = predicate(items[i].get());
    }

    [[nodiscard]] auto getItems() const noexcept
    {
        return ranges::views::transform(items, [](const auto& item) -> decltype(auto) { return item.get(); }) |
               ranges::views::enumerate |
               ranges::views::filter([this](const auto& pair) { return passesFilter[std::get<0>(pair)]; });
    }

    [[nodiscard]] std::size_t totalItemCount() const noexcept
    {
        return items.size();
    }

private:
    // wrapper around bool to not use std::vector<bool>
    // which isn't compatible with ranges::sort()
    struct Boolean {
        explicit(false) Boolean(bool b) : b{ b } {}

        explicit(false) operator bool() const noexcept
        {
            return b;
        }

    private:
        bool b;
    };

    std::vector<std::reference_wrapper<const ItemType>> items;
    std::vector<Boolean> passesFilter;
};
