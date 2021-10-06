#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

class ProtobufReader {
public:
    ProtobufReader(const std::uint8_t* data, int size) : data{ data }, size{ size } {}

    [[nodiscard]] std::int32_t readInt32(std::uint8_t index) const noexcept
    {
        int offset = 0;
        while (offset + 1 < size) {
            if (const auto key = readVarint(offset); wireType(key) == WireType::Varint && (key >> 3) == index)
                return readVarint(offset + 1);
            ++offset;
        }
        return -1;
    }

    [[nodiscard]] std::vector<std::string> readRepeatedString(std::uint8_t index) const noexcept
    {
        std::vector<std::string> strings;
        int offset = 0;
        while (offset + 1 < size) {
            if (const auto key = readVarint(offset); wireType(key) == WireType::LengthDelimited && (key >> 3) == index) {
                offset += strings.emplace_back(readString(offset + 1)).length();
            }
            ++offset;
        }
        return strings;
    }

private:
    [[nodiscard]] std::int8_t readVarint(int offset) const noexcept
    {
        assert(offset >= 0 && offset < size);
        assert((data[offset] & 0x80) == 0 && "Only one-byte varints are supported!");
        return data[offset] & 0x7F;
    }

    [[nodiscard]] std::string readString(int offset) const noexcept
    {
        assert(offset >= 0 && offset < size);
        const auto length = readVarint(offset);
        return (length > 0 && offset + 1 + length < size) ? std::string{ reinterpret_cast<const char*>(data + offset + 1), static_cast<std::size_t>(length) } : std::string{};
    }

    enum class WireType {
        Varint = 0,
        LengthDelimited = 2
    };

    [[nodiscard]] constexpr WireType wireType(std::int8_t key) const noexcept
    {
        return static_cast<WireType>(key & 3);
    }

    const std::uint8_t* data;
    int size;
};
