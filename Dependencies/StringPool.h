/*
StringPool - A performant and memory efficient storage for immutable strings with C++17.
Supports all standard char types: char, wchar_t, char16_t, char32_t and C++20's char8_t.

MIT License

Copyright (c) 2021-2022 Daniel Krupiński

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

template <typename T, bool NullTerminateStrings = true>
class StringBlock;

template <typename BlockIterator>
class StringBlockEditor;

template <typename T, bool NullTerminateStrings = true>
class StringPool {
    using BlockType = StringBlock<T, NullTerminateStrings>;
    using Blocks = std::vector<BlockType>;

public:
    StringPool() = default;
    explicit StringPool(std::size_t standardBlockCapacity) noexcept(std::is_nothrow_default_constructible_v<Blocks>) : standardBlockCapacity{ standardBlockCapacity } {}

    template <typename ...Pools, typename = std::enable_if_t<std::conjunction_v<std::is_same<Pools, StringPool>...>>>
    StringPool(StringPool&& largestPool, Pools&&... pools) : blocks{ std::move(largestPool.blocks) }
    {
        blocks.reserve(blocks.size() + (pools.blocks.size() + ...));

        BlockIterator inserted;
        (((inserted = blocks.insert(blocks.end(), std::make_move_iterator(pools.blocks.begin()), std::make_move_iterator(pools.blocks.end()))),
           pools.blocks.clear(),
           std::inplace_merge(blocks.begin(), inserted, blocks.end(), [](const auto& a, const auto& b) { return a.getFreeSpace() < b.getFreeSpace(); })), ...);
    }

    using StringType = typename BlockType::StringType;

    [[nodiscard]] StringType add(StringType string)
    {
        return getBlockCapableOfStoringStringOfLength(string.length()).addString(string);
    }

    [[nodiscard]] std::size_t getBlockCount() const noexcept
    {
        return blocks.size();
    }

    [[nodiscard]] std::size_t getStandardBlockCapacity() const noexcept
    {
        return standardBlockCapacity;
    }

    void setStandardBlockCapacity(std::size_t newStandardBlockCapacity) noexcept
    {
        standardBlockCapacity = newStandardBlockCapacity;
    }

private:
    using BlockIterator = typename Blocks::iterator;

    [[nodiscard]] StringBlockEditor<BlockIterator> getBlockCapableOfStoringStringOfLength(std::size_t length)
    {
        const auto block = findOrCreateBlockCapableOfStoringStringOfLength(length);
        return { blocks.begin(), block };
    }

    [[nodiscard]] BlockIterator findOrCreateBlockCapableOfStoringStringOfLength(std::size_t length)
    {
        if (const auto blockCapableOfStoringString = findBlockCapableOfStoringStringOfLength(length); blockCapableOfStoringString != blocks.end())
            return blockCapableOfStoringString;
        return createBlockCapableOfStoringStringOfLength(length);
    }

    [[nodiscard]] BlockIterator findBlockCapableOfStoringStringOfLength(std::size_t length)
    {
        return std::lower_bound(getFirstBlockMaybeCapableOfStoringStringOfLength(length), blocks.end(), true, [length](const auto& block, bool) { return !block.canTakeStringOfLength(length); });
    }

    [[nodiscard]] BlockIterator createBlockCapableOfStoringStringOfLength(std::size_t length)
    {
        blocks.emplace_back((std::max)(standardBlockCapacity, BlockType::getSpaceRequiredToStoreStringOfLength(length)));
        return std::prev(blocks.end());
    }

    [[nodiscard]] BlockIterator getFirstBlockMaybeCapableOfStoringStringOfLength(std::size_t length)
    {
        const BlockIterator begin = blocks.begin(), end = blocks.end();
        if (std::distance(begin, end) > 2 && !std::prev(end, 2)->canTakeStringOfLength(length))
            return std::prev(end);
        return begin;
    }

    Blocks blocks;
    std::size_t standardBlockCapacity = 8192;
};

template <typename T, bool NullTerminateStrings>
class StringBlock {
public:
    explicit StringBlock(std::size_t elementCount) : memory{ new T[elementCount] }, size{ elementCount }, freeSpace{ elementCount } {}

    using StringType = std::basic_string_view<T>;

    [[nodiscard]] StringType addString(StringType string)
    {
        if (!canTakeStringOfLength(string.length())) {
            assert(false && "StringBlock doesn't have enough capacity to store the string");
            return &nullChar;
        }

        const auto destination = memory.get() + getUsedSpace();
        std::copy(string.begin(), string.end(), destination);

        if constexpr (NullTerminateStrings)
            destination[string.length()] = 0;

        freeSpace -= getSpaceRequiredToStoreStringOfLength(string.length());
        return { destination, string.length() };
    }

    [[nodiscard]] bool canTakeStringOfLength(std::size_t length) const noexcept
    {
        return getFreeSpace() >= getSpaceRequiredToStoreStringOfLength(length) && isStringLengthValid(length);
    }

    [[nodiscard]] static constexpr std::size_t getSpaceRequiredToStoreStringOfLength(std::size_t length) noexcept
    {
        if constexpr (NullTerminateStrings)
            return isStringLengthValid(length) ? length + 1 : length;
        else
            return length;
    }

    [[nodiscard]] std::size_t getFreeSpace() const noexcept
    {
        return freeSpace;
    }

    friend void swap(StringBlock& a, StringBlock& b) noexcept
    {
        a.memory.swap(b.memory);
        std::swap(a.size, b.size);
        std::swap(a.freeSpace, b.freeSpace);
    }

    [[nodiscard]] static constexpr bool nullTerminatesStrings() noexcept
    {
        return NullTerminateStrings;
    }

private:
    [[nodiscard]] std::size_t getUsedSpace() const noexcept
    {
        return size - freeSpace;
    }

    [[nodiscard]] static constexpr bool isStringLengthValid(std::size_t length) noexcept
    {
        if constexpr (NullTerminateStrings)
            return length != (std::numeric_limits<std::size_t>::max)();
        else
            return true;
    }

    static constexpr T nullChar = 0;

    std::unique_ptr<T[]> memory;
    std::size_t size = 0;
    std::size_t freeSpace = 0;
};

template <typename BlockIterator>
class StringBlockEditor {
public:
    StringBlockEditor(BlockIterator firstBlock, BlockIterator editedBlock) : firstBlock{ firstBlock }, editedBlock{ editedBlock } {}

    template <typename StringType>
    [[nodiscard]] StringType addString(StringType string)
    {
        return editedBlock->addString(string);
    }

    ~StringBlockEditor()
    {
        if (shouldReorderBlocksAfterAddingStringToBlock())
            reorderBlocksAfterAddingStringToBlock();
    }

private:
    [[nodiscard]] bool shouldReorderBlocksAfterAddingStringToBlock() const
    {
        return editedBlock != firstBlock && editedBlock->getFreeSpace() < std::prev(editedBlock)->getFreeSpace();
    }

    void reorderBlocksAfterAddingStringToBlock() const
    {
        moveEditedBlockInPlaceOf(getFirstBlockWithMoreFreeSpaceThanEditedBlock());
    }

    [[nodiscard]] BlockIterator getFirstBlockWithMoreFreeSpaceThanEditedBlock() const
    {
        return std::upper_bound(firstBlock, editedBlock, editedBlock->getFreeSpace(), [](const auto freeSpace, const auto& block) { return freeSpace < block.getFreeSpace(); });
    }
    
    void moveEditedBlockInPlaceOf(BlockIterator block) const
    {
        if (block == editedBlock)
            return;

        if (block->getFreeSpace() == std::prev(editedBlock)->getFreeSpace()) {
            std::iter_swap(block, editedBlock);
        } else {
            while (block != editedBlock) {
                std::iter_swap(block, editedBlock);
                ++block;
            }
        }
    }

    BlockIterator firstBlock;
    BlockIterator editedBlock;
};
