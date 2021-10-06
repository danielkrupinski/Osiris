// dear imgui: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bits indices.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2021-02-18: DirectX9: Change blending equation to preserve alpha in output buffer.
//  2019-05-29: DirectX9: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: DirectX9: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-03-29: Misc: Fixed erroneous assert in ImGui_ImplDX9_InvalidateDeviceObjects().
//  2019-01-16: Misc: Disabled fog before drawing UI's. Fixes issue #2288.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-06-08: Misc: Extracted imgui_impl_dx9.cpp/.h away from the old combined DX9+Win32 example.
//  2018-06-08: DirectX9: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-05-07: Render: Saving/restoring Transform because they don't seem to be included in the StateBlock. Setting shading mode to Gouraud.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplDX9_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.

#include <cstdint>
#include <d3d9.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

#include "imgui.h"
#include "imgui_impl_dx9.h"

#include "../Resources/Shaders/Build/default_vs.h"

// DirectX data
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER9   g_pIB = NULL;
static LPDIRECT3DTEXTURE9       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

static ComPtr<IDirect3DVertexShader9> vertexShader;
static ComPtr<IDirect3DVertexDeclaration9> vertexDeclaration;

static void ImGui_ImplDX9_SetupRenderState(ImDrawData* draw_data)
{
    // Setup viewport
    D3DVIEWPORT9 vp;
    vp.X = vp.Y = 0;
    vp.Width = (DWORD)draw_data->DisplaySize.x;
    vp.Height = (DWORD)draw_data->DisplaySize.y;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    g_pd3dDevice->SetViewport(&vp);

    // Setup render state: alpha-blending, no face culling, no depth testing, shade mode (for gradient)
    g_pd3dDevice->SetPixelShader(NULL);
    g_pd3dDevice->SetVertexShader(vertexShader.Get());
    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
    g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    // Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
    {
        float L = draw_data->DisplayPos.x + 0.5f;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
        float T = draw_data->DisplayPos.y + 0.5f;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;
        D3DMATRIX mat_projection =
        { { {
            2.0f / (R - L),   0.0f,         0.0f,  0.0f,
            0.0f,         2.0f / (T - B),   0.0f,  0.0f,
            0.0f,         0.0f,         0.5f,  0.0f,
            (L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f
        } } };
        g_pd3dDevice->SetVertexShaderConstantF(0, &mat_projection.m[0][0], 4);
    }
}

// Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
void ImGui_ImplDX9_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(ImDrawVert), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
            return;
    }

    // Backup the DX9 state
    ComPtr<IDirect3DStateBlock9> d3d9_state_block;
    if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, d3d9_state_block.GetAddressOf()) != D3D_OK || d3d9_state_block->Capture() != D3D_OK)
        return;

    ImDrawVert* vtx_dst;
    if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(ImDrawVert)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
        return;

    ImDrawIdx* idx_dst;
    if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
        return;

    for (int n = 0; n < draw_data->CmdListsCount; ++n) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        vtx_dst += cmd_list->VtxBuffer.Size;
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }

    g_pVB->Unlock();
    g_pIB->Unlock();
    g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(ImDrawVert));
    g_pd3dDevice->SetIndices(g_pIB);
    g_pd3dDevice->SetVertexDeclaration(vertexDeclaration.Get());

    // Setup desired DX state
    ImGui_ImplDX9_SetupRenderState(draw_data);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDX9_SetupRenderState(draw_data);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            } else
            {
                const RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
                const LPDIRECT3DTEXTURE9 texture = (LPDIRECT3DTEXTURE9)pcmd->TextureId;
                g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, texture == g_FontTexture ? D3DTOP_SELECTARG2 : D3DTOP_MODULATE);
                g_pd3dDevice->SetTexture(0, texture);
                g_pd3dDevice->SetScissorRect(&r);
                g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, pcmd->VtxOffset + global_vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, pcmd->IdxOffset + global_idx_offset, pcmd->ElemCount / 3);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore the DX9 state
    d3d9_state_block->Apply();
}

bool ImGui_ImplDX9_Init(IDirect3DDevice9* device)
{
    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_dx9";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

    LoadLibraryW(L"d3d9"); // increment d3d9.dll reference count

    g_pd3dDevice = device;
    g_pd3dDevice->AddRef();
    return true;
}

void ImGui_ImplDX9_Shutdown()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    // Upload texture to graphics system
    g_FontTexture = NULL;
    if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
        return false;
    D3DLOCKED_RECT tex_locked_rect;
    if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, D3DLOCK_DISCARD) != D3D_OK)
        return false;
    for (int y = 0; y < height; y++)
        memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + width * y, width);
    g_FontTexture->UnlockRect(0);

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)g_FontTexture;

    return true;
}

void ImGui_ImplDX9_DestroyFontsTexture()
{
    if (g_FontTexture) { g_FontTexture->Release(); g_FontTexture = NULL; ImGui::GetIO().Fonts->TexID = NULL; } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
    if (!g_pd3dDevice)
        return false;
    if (!ImGui_ImplDX9_CreateFontsTexture())
        return false;

    if (!vertexDeclaration.Get()) {
        constexpr D3DVERTEXELEMENT9 elements[]{
            { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
            { 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
            D3DDECL_END()
        };
        g_pd3dDevice->CreateVertexDeclaration(elements, vertexDeclaration.GetAddressOf());
    }

    if (!vertexShader.Get())
        g_pd3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(default_vs), vertexShader.GetAddressOf());

    return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
    if (!g_pd3dDevice)
        return;
    if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
    if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
    vertexShader.Reset();
    ImGui_ImplDX9_DestroyFontsTexture();
}

void* ImGui_CreateTextureRGBA(int width, int height, const unsigned char* data)
{
    IDirect3DTexture9* tempTexture;
    if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tempTexture, nullptr) != D3D_OK)
        return nullptr;

    D3DLOCKED_RECT lockedRect;
    if (tempTexture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD) != D3D_OK) {
        tempTexture->Release();
        return nullptr;
    }

    for (int y = 0; y < height; ++y) {
        const auto src = reinterpret_cast<const std::uint32_t*>(data + width * 4 * y);
        const auto dest = reinterpret_cast<std::uint32_t*>((unsigned char*)lockedRect.pBits + lockedRect.Pitch * y);

        // RGBA --> BGRA
        for (int i = 0; i < width; ++i) {
            auto color = src[i];
            color = (color & 0xFF00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0xFF) << 16);
            dest[i] = color;
        }
    }

    tempTexture->UnlockRect(0);

    IDirect3DTexture9* texture;
    if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, nullptr) != D3D_OK) {
        tempTexture->Release();
        return nullptr;
    }

    g_pd3dDevice->UpdateTexture(tempTexture, texture);
    tempTexture->Release();

    return texture;
}

void ImGui_DestroyTexture(void* texture)
{
    reinterpret_cast<IDirect3DTexture9*>(texture)->Release();
}

void ImGui_ImplDX9_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplDX9_CreateDeviceObjects();
}
