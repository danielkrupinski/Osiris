#ifdef _WIN32
#include <d3d9.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
#else
#include "imgui/GL/gl3w.h"
#endif

#include "imgui/imgui.h"

#ifdef _WIN32
// shaders are build during compilation and header files are created
#ifdef _DEBUG
#include "Resources/Shaders/Build/Debug/blur_x.h"
#include "Resources/Shaders/Build/Debug/blur_y.h"
#include "Resources/Shaders/Build/Debug/chromatic_aberration.h"
#include "Resources/Shaders/Build/Debug/monochrome.h"
#else
#include "Resources/Shaders/Build/Release/blur_x.h"
#include "Resources/Shaders/Build/Release/blur_y.h"
#include "Resources/Shaders/Build/Release/chromatic_aberration.h"
#include "Resources/Shaders/Build/Release/monochrome.h"
#endif

#endif

#include "PostProcessing.h"

static int backbufferWidth = 0;
static int backbufferHeight = 0;

#ifdef _WIN32
static IDirect3DDevice9* device; // DO NOT RELEASE!

[[nodiscard]] static IDirect3DTexture9* createTexture(int width, int height) noexcept
{
    IDirect3DTexture9* texture;
    device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, nullptr);
    return texture;
}

static void copyBackbufferToTexture(IDirect3DTexture9* texture, D3DTEXTUREFILTERTYPE filtering) noexcept
{
    if (ComPtr<IDirect3DSurface9> backBuffer; device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, backBuffer.GetAddressOf()) == D3D_OK) {
        if (ComPtr<IDirect3DSurface9> surface; texture->GetSurfaceLevel(0, surface.GetAddressOf()) == D3D_OK)
            device->StretchRect(backBuffer.Get(), nullptr, surface.Get(), nullptr, filtering);
    }
}

static void setRenderTarget(IDirect3DTexture9* rtTexture) noexcept
{
    if (ComPtr<IDirect3DSurface9> surface; rtTexture->GetSurfaceLevel(0, surface.GetAddressOf()) == D3D_OK)
        device->SetRenderTarget(0, surface.Get());
}

#else
[[nodiscard]] static GLuint createTexture(int width, int height) noexcept
{
    GLint lastTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, lastTexture);
    return texture;
}
#endif

class ShaderProgram {
public:
    ~ShaderProgram()
    {
#ifndef _WIN32
        if (program)
            glDeleteProgram(program);
#endif
    }

    void use(float uniform, int location) const noexcept
    {
#ifdef _WIN32
        device->SetPixelShader(pixelShader.Get());
        const float params[4] = { uniform };
        device->SetPixelShaderConstantF(location, params, 1);
#else
        glUseProgram(program);
        glUniform1f(location, uniform);
#endif
    }

#ifdef _WIN32
    void init(const BYTE* pixelShaderSrc) noexcept
#else
    void init(const char* pixelShaderSrc, const char* vertexShaderSrc) noexcept
#endif
    {
        if (initialized)
            return;
        initialized = true;

#ifdef _WIN32
        device->CreatePixelShader(reinterpret_cast<const DWORD*>(pixelShaderSrc), pixelShader.GetAddressOf());
#else
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &pixelShaderSrc, nullptr);
        glCompileShader(fragmentShader);

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);

        program = glCreateProgram();
        glAttachShader(program, fragmentShader);
        glAttachShader(program, vertexShader);
        glLinkProgram(program);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
#endif
    }

private:
#ifdef _WIN32
    ComPtr<IDirect3DPixelShader9> pixelShader;
#else
    GLuint program = 0;
#endif
    bool initialized = false;
};

class BlurEffect {
public:
    static void draw(ImDrawList* drawList, float alpha) noexcept
    {
        instance()._draw(drawList, alpha);
    }

#ifdef _WIN32
    static void clearTextures() noexcept
    {
        if (instance().blurTexture1) {
            instance().blurTexture1->Release();
            instance().blurTexture1 = nullptr;
        }
        if (instance().blurTexture2) {
            instance().blurTexture2->Release();
            instance().blurTexture2 = nullptr;
        }
    }
#else
    static void clearTextures() noexcept
    {
        if (instance().blurTexture1) {
            glDeleteTextures(1, &instance().blurTexture1);
            instance().blurTexture1 = 0;
        }
        if (instance().blurTexture2) {
            glDeleteTextures(1, &instance().blurTexture2);
            instance().blurTexture2 = 0;
        }
    }
#endif
private:
#ifdef _WIN32
    IDirect3DSurface9* rtBackup = nullptr;
    IDirect3DTexture9* blurTexture1 = nullptr;
    IDirect3DTexture9* blurTexture2 = nullptr;
#else
    GLint textureBackup = 0;
    GLint fboBackup = 0;
    GLint programBackup = 0;

    GLuint blurTexture1 = 0;
    GLuint blurTexture2 = 0;
    GLuint frameBuffer = 0;
#endif

    ShaderProgram blurShaderX;
    ShaderProgram blurShaderY;
    static constexpr auto blurDownsample = 4;

    BlurEffect() = default;
    BlurEffect(const BlurEffect&) = delete;

    ~BlurEffect()
    {
#ifdef _WIN32
        if (rtBackup)
            rtBackup->Release();
        if (blurTexture1)
            blurTexture1->Release();
        if (blurTexture2)
            blurTexture2->Release();
#endif
    }

    static BlurEffect& instance() noexcept
    {
        static BlurEffect blurEffect;
        return blurEffect;
    }

    static void begin(const ImDrawList*, const ImDrawCmd*) noexcept { instance()._begin(); }
    static void firstPass(const ImDrawList*, const ImDrawCmd*) noexcept { instance()._firstPass(); }
    static void secondPass(const ImDrawList*, const ImDrawCmd*) noexcept { instance()._secondPass(); }
    static void end(const ImDrawList*, const ImDrawCmd*) noexcept { instance()._end(); }

    void createTextures() noexcept
    {
        if (!blurTexture1)
            blurTexture1 = createTexture(backbufferWidth / blurDownsample, backbufferHeight / blurDownsample);
        if (!blurTexture2)
            blurTexture2 = createTexture(backbufferWidth / blurDownsample, backbufferHeight / blurDownsample);
    }

    void createShaders() noexcept
    {
#ifdef _WIN32
        blurShaderX.init(blur_x);
        blurShaderY.init(blur_y);
#else
        blurShaderX.init(
#include "Resources/Shaders/blur_x.glsl"
            ,
#include "Resources/Shaders/passthrough.glsl"
        );

        blurShaderY.init(
#include "Resources/Shaders/blur_y.glsl"
            ,
#include "Resources/Shaders/passthrough.glsl"
        );
#endif
    }

    void _begin() noexcept
    {
#ifdef _WIN32
        device->GetRenderTarget(0, &rtBackup);

        copyBackbufferToTexture(blurTexture1, D3DTEXF_LINEAR);

        device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

        const D3DMATRIX projection{ {{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f / (backbufferWidth / blurDownsample), 1.0f / (backbufferHeight / blurDownsample), 0.0f, 1.0f
        }} };
        device->SetVertexShaderConstantF(0, &projection.m[0][0], 4);
#else
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureBackup);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fboBackup);
        glGetIntegerv(GL_CURRENT_PROGRAM, &programBackup);

        if (!frameBuffer)
            glGenFramebuffers(1, &frameBuffer);

        glViewport(0, 0, backbufferWidth / blurDownsample, backbufferHeight / blurDownsample);
        glDisable(GL_SCISSOR_TEST);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture1, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, blurTexture2, 0);
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, backbufferWidth, backbufferHeight, 0, 0, backbufferWidth / blurDownsample, backbufferHeight / blurDownsample, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
#endif
    }

    void _firstPass() noexcept
    {
        blurShaderX.use(1.0f / (backbufferWidth / blurDownsample), 0);
#ifdef _WIN32
        setRenderTarget(blurTexture2);
#else
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glUniform1i(1, 0);
#endif
    }

    void _secondPass() noexcept
    {
        blurShaderY.use(1.0f / (backbufferHeight / blurDownsample), 0);
#ifdef _WIN32
        setRenderTarget(blurTexture1);
#else
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glUniform1i(1, 0);
#endif
    }

    void _end() noexcept
    {
#ifdef _WIN32
        device->SetRenderTarget(0, rtBackup);
        rtBackup->Release();

        device->SetPixelShader(nullptr);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
#else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboBackup);
        glUseProgram(programBackup);
        glBindTexture(GL_TEXTURE_2D, textureBackup);
#endif
    }

    void _draw(ImDrawList* drawList, float alpha) noexcept
    {
        createTextures();
        createShaders();

        if (!blurTexture1 || !blurTexture2)
            return;

        drawList->AddCallback(&begin, nullptr);
        for (int i = 0; i < 8; ++i) {
            drawList->AddCallback(&firstPass, nullptr);
            drawList->AddImage(reinterpret_cast<ImTextureID>(blurTexture1), { -1.0f, -1.0f }, { 1.0f, 1.0f });
            drawList->AddCallback(&secondPass, nullptr);
            drawList->AddImage(reinterpret_cast<ImTextureID>(blurTexture2), { -1.0f, -1.0f }, { 1.0f, 1.0f });
        }
        drawList->AddCallback(&end, nullptr);
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

#ifdef _WIN32
        drawList->AddImage(reinterpret_cast<ImTextureID>(blurTexture1), { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32(255, 255, 255, 255 * alpha));
#else
        drawList->AddImage(reinterpret_cast<ImTextureID>(blurTexture1), { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, IM_COL32(255, 255, 255, 255 * alpha));
#endif
    }
};

class ChromaticAberration {
public:
    static void draw(ImDrawList* drawList, float amount) noexcept
    {
        instance().amount = amount;
        instance()._draw(drawList);
    }

    static void clearTexture() noexcept
    {
        instance()._clearTexture();
    }

private:
#ifdef _WIN32
    IDirect3DTexture9* texture = nullptr;
#else
    GLint textureBackup = 0;
    GLint programBackup = 0;

    GLuint texture = 0;
    GLuint frameBuffer = 0;
#endif

    ShaderProgram shader;
    float amount = 0.0f;

    ChromaticAberration() = default;
    ChromaticAberration(const ChromaticAberration&) = delete;

    ~ChromaticAberration()
    {
#ifdef _WIN32
        if (texture)
            texture->Release();
#endif
    }

    static ChromaticAberration& instance() noexcept
    {
        static ChromaticAberration chromaticAberration;
        return chromaticAberration;
    }

    void _clearTexture() noexcept
    {
#ifdef _WIN32
        if (texture) {
            texture->Release();
            texture = nullptr;
        }
#else
        if (texture) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
#endif
    }

    static void begin(const ImDrawList*, const ImDrawCmd* cmd) noexcept { instance()._begin(); }
    static void end(const ImDrawList*, const ImDrawCmd* cmd) noexcept { instance()._end(); }

    void createTexture() noexcept
    {
        if (!texture)
            texture = ::createTexture(backbufferWidth, backbufferHeight);
    }

    void createShaders() noexcept
    {
#ifdef _WIN32
        shader.init(chromatic_aberration);
#else
        shader.init(
#include "Resources/Shaders/chromatic_aberration.glsl"
            ,
#include "Resources/Shaders/passthrough.glsl"
        );
#endif
    }

    void _begin() noexcept
    {
#ifdef _WIN32
        copyBackbufferToTexture(texture, D3DTEXF_NONE);

        device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

        const D3DMATRIX projection{ {{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f / (backbufferWidth), 1.0f / (backbufferHeight), 0.0f, 1.0f
        }} };
        device->SetVertexShaderConstantF(0, &projection.m[0][0], 4);
#else
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureBackup);
        glGetIntegerv(GL_CURRENT_PROGRAM, &programBackup);

        if (!frameBuffer) {
            glGenFramebuffers(1, &frameBuffer);
        }

        glDisable(GL_SCISSOR_TEST);

        GLint fboBackup = 0;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fboBackup);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, backbufferWidth, backbufferHeight, 0, 0, backbufferWidth, backbufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboBackup);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
#endif
        shader.use(amount, 0);
#ifndef _WIN32
        glUniform1i(1, 0);
#endif
    }

    void _end() noexcept
    {
#ifdef _WIN32
        device->SetPixelShader(nullptr);
#else
        glUseProgram(programBackup);
        glBindTexture(GL_TEXTURE_2D, textureBackup);
        glEnable(GL_SCISSOR_TEST);
#endif
    }

    void _draw(ImDrawList* drawList) noexcept
    {
        createTexture();
        createShaders();
        if (!texture)
            return;

        drawList->AddCallback(&begin, nullptr);
        drawList->AddImage(reinterpret_cast<ImTextureID>(texture), { -1.0f, -1.0f }, { 1.0f, 1.0f });
        drawList->AddCallback(&end, nullptr);
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
    }
};

class MonochromeEffect {
public:
    static void draw(ImDrawList* drawList, float amount) noexcept
    {
        instance().amount = amount;
        instance()._draw(drawList);
    }

    static void clearTexture() noexcept
    {
        instance()._clearTexture();
    }

private:
#ifdef _WIN32
    IDirect3DTexture9* texture = nullptr;
#else
    GLint textureBackup = 0;
    GLint programBackup = 0;

    GLuint texture = 0;
    GLuint frameBuffer = 0;
#endif

    ShaderProgram shader;
    float amount = 0.0f;

    MonochromeEffect() = default;
    MonochromeEffect(const MonochromeEffect&) = delete;

    ~MonochromeEffect()
    {
#ifdef _WIN32
        if (texture)
            texture->Release();
#endif
    }

    static MonochromeEffect& instance() noexcept
    {
        static MonochromeEffect monochromeEffect;
        return monochromeEffect;
    }

    void _clearTexture() noexcept
    {
#ifdef _WIN32
        if (texture) {
            texture->Release();
            texture = nullptr;
        }
#else
        if (texture) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
#endif
    }

    static void begin(const ImDrawList*, const ImDrawCmd* cmd) noexcept { instance()._begin(); }
    static void end(const ImDrawList*, const ImDrawCmd* cmd) noexcept { instance()._end(); }

    void createTexture() noexcept
    {
        if (!texture)
            texture = ::createTexture(backbufferWidth, backbufferHeight);
    }

    void createShaders() noexcept
    {
#ifdef _WIN32
        shader.init(monochrome);
#else
        shader.init(
#include "Resources/Shaders/monochrome.glsl"
            ,
#include "Resources/Shaders/passthrough.glsl"
        );
#endif
    }

    void _begin() noexcept
    {
#ifdef _WIN32
        copyBackbufferToTexture(texture, D3DTEXF_NONE);

        device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

        const D3DMATRIX projection{ {{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f / (backbufferWidth), 1.0f / (backbufferHeight), 0.0f, 1.0f
        }} };
        device->SetVertexShaderConstantF(0, &projection.m[0][0], 4);
#else
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureBackup);
        glGetIntegerv(GL_CURRENT_PROGRAM, &programBackup);

        if (!frameBuffer) {
            glGenFramebuffers(1, &frameBuffer);
        }

        glDisable(GL_SCISSOR_TEST);

        GLint fboBackup = 0;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fboBackup);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, backbufferWidth, backbufferHeight, 0, 0, backbufferWidth, backbufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboBackup);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
#endif
        shader.use(amount, 0);
#ifndef _WIN32
        glUniform1i(1, 0);
#endif
    }

    void _end() noexcept
    {
#ifdef _WIN32
        device->SetPixelShader(nullptr);
#else
        glUseProgram(programBackup);
        glBindTexture(GL_TEXTURE_2D, textureBackup);
        glEnable(GL_SCISSOR_TEST);
#endif
    }

    void _draw(ImDrawList* drawList) noexcept
    {
        createTexture();
        createShaders();
        if (!texture)
            return;

        drawList->AddCallback(&begin, nullptr);
        drawList->AddImage(reinterpret_cast<ImTextureID>(texture), { -1.0f, -1.0f }, { 1.0f, 1.0f });
        drawList->AddCallback(&end, nullptr);
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
    }
};

#ifdef _WIN32
void PostProcessing::setDevice(IDirect3DDevice9* device) noexcept
{
    ::device = device;
}

void PostProcessing::clearBlurTextures() noexcept
{
    BlurEffect::clearTextures();
}

void PostProcessing::onDeviceReset() noexcept
{
    BlurEffect::clearTextures();
    ChromaticAberration::clearTexture();
}
#endif

void PostProcessing::newFrame() noexcept
{
    if (const auto [width, height] = ImGui::GetIO().DisplaySize; backbufferWidth != static_cast<int>(width) || backbufferHeight != static_cast<int>(height)) {
        BlurEffect::clearTextures();
        ChromaticAberration::clearTexture();
        MonochromeEffect::clearTexture();
        backbufferWidth = static_cast<int>(width);
        backbufferHeight = static_cast<int>(height);
    }
}

void PostProcessing::performFullscreenBlur(ImDrawList* drawList, float alpha) noexcept
{
    BlurEffect::draw(drawList, alpha);
}

void PostProcessing::performFullscreenChromaticAberration(ImDrawList* drawList, float amount) noexcept
{
    ChromaticAberration::draw(drawList, amount);
}

void PostProcessing::performFullscreenMonochrome(ImDrawList* drawList, float amount) noexcept
{
    MonochromeEffect::draw(drawList, amount);
}
