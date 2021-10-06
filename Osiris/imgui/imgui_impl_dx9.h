// dear imgui: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID in imgui.cpp.
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bits indices.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once

struct IDirect3DDevice9;
struct ImDrawData;

bool     ImGui_ImplDX9_Init(IDirect3DDevice9* device);
void     ImGui_ImplDX9_Shutdown();
void     ImGui_ImplDX9_NewFrame();
void     ImGui_ImplDX9_RenderDrawData(ImDrawData* draw_data);

void     ImGui_ImplDX9_DestroyFontsTexture();

// Use if you want to reset your rendering device without losing ImGui state.
bool     ImGui_ImplDX9_CreateDeviceObjects();
void     ImGui_ImplDX9_InvalidateDeviceObjects();

void*    ImGui_CreateTextureRGBA(int width, int height, const unsigned char* data);
void     ImGui_DestroyTexture(void* texture);
