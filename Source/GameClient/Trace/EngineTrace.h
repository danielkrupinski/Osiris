#pragma once

#include <CS2/Classes/Vector.h>
#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()
#include <Windows.h>
#endif

// Minimal CS2 trace structures
struct Ray_t {
    cs2::Vector m_Start;
    cs2::Vector m_Delta;
    cs2::Vector m_StartOffset;
    cs2::Vector m_Extents;
    bool m_IsRay;
    bool m_IsSwept;

    void Init(const cs2::Vector& start, const cs2::Vector& end) noexcept
    {
        m_Start = start;
        m_Delta.x = end.x - start.x;
        m_Delta.y = end.y - start.y;
        m_Delta.z = end.z - start.z;
        m_StartOffset = cs2::Vector{};
        m_Extents = cs2::Vector{};
        m_IsRay = true;
        m_IsSwept = false;
    }
};

struct CGameTrace {
    cs2::Vector startpos;
    cs2::Vector endpos;
    void* plane{};
    float fraction{1.0f};
    int contents{};
    std::uint16_t dispFlags{};
    bool allsolid{};
    bool startsolid{};
    void* m_pEnt{};
};

// Non-virtual trace filter (avoids CRT _purecall / type_info dependencies)
struct TraceFilterVTable {
    void* shouldHitEntity;
    void* getTraceType;
};

struct SimpleTraceFilter {
    TraceFilterVTable* vtable;
    void* skipEntity;
};

#if IS_WIN64()
inline bool ShouldHitEntityImpl(SimpleTraceFilter* self, void* entity, int) noexcept
{
    return entity != self->skipEntity;
}
inline int GetTraceTypeImpl(SimpleTraceFilter*) noexcept { return 0; }

inline TraceFilterVTable g_filterVTable{reinterpret_cast<void*>(&ShouldHitEntityImpl),
                                         reinterpret_cast<void*>(&GetTraceTypeImpl)};
#endif

// VTable layout for IEngineTrace (Source 2):
//   [0] dtor, [1] GetPointContents, [2] GetPointContentsWorld,
//   [3] GetPointContentsCollideable, [4] TraceRay
inline constexpr unsigned int MASK_SHOT = 0x1C1003;

class EngineTrace {
public:
    [[nodiscard]] bool init() noexcept
    {
#if IS_WIN64()
        const auto engineModule = GetModuleHandleA("engine2.dll");
        if (!engineModule)
            return false;

        using CreateInterfaceFn = void* (*)(const char*, int*);
        const auto createInterface = reinterpret_cast<CreateInterfaceFn>(
            GetProcAddress(engineModule, "CreateInterface"));
        if (!createInterface)
            return false;

        m_interface = createInterface("EngineTraceClient004", nullptr);
        if (!m_interface)
            return false;

        m_vtable = *reinterpret_cast<void***>(m_interface);
        return m_vtable != nullptr;
#else
        return false;
#endif
    }

    [[nodiscard]] bool isVisible(const cs2::Vector& from, const cs2::Vector& to,
                                  void* skipEntity) const noexcept
    {
#if IS_WIN64()
        if (!m_vtable)
            return true;

        Ray_t ray;
        ray.Init(from, to);
        SimpleTraceFilter filter{&g_filterVTable, skipEntity};
        CGameTrace trace{};

        using TraceRayFn = void(*)(void*, const Ray_t&, unsigned int, void*, CGameTrace*);
        const auto traceRay = reinterpret_cast<TraceRayFn>(m_vtable[4]);
        traceRay(m_interface, ray, MASK_SHOT, &filter, &trace);

        return trace.fraction > 0.97f;
#else
        return true;
#endif
    }

private:
    void* m_interface{nullptr};
    void** m_vtable{nullptr};
};
