#include "Texture.h"

#ifdef _WIN32
#include "imgui/imgui_impl_dx9.h"
#else
#include "imgui/imgui_impl_opengl3.h"
#endif

void Texture::init(int width, int height, const std::uint8_t* data) noexcept
{
    texture = ImGui_CreateTextureRGBA(width, height, data);
}

void Texture::clear() noexcept
{
    if (texture)
        ImGui_DestroyTexture(texture);
    texture = nullptr;
}
