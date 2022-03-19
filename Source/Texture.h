#pragma once

#include <cstdint>

using ImTextureID = void*;

class Texture {
    ImTextureID texture = nullptr;
public:
    Texture() = default;
    ~Texture() { clear(); }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept : texture{ other.texture } { other.texture = nullptr; }
    Texture& operator=(Texture&& other) noexcept { clear(); texture = other.texture; other.texture = nullptr; return *this; }

    void init(int width, int height, const std::uint8_t* data) noexcept;
    void clear() noexcept;
    [[nodiscard]] ImTextureID get() const noexcept { return texture; }
};
