#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Vector.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

namespace engine_trace {
struct OutputStorage;
struct Bounds6f;
}

STRONG_TYPE_ALIAS(ShapeBuilderFunctionPointer, void(*)(void*, const engine_trace::Bounds6f*) noexcept);
STRONG_TYPE_ALIAS(TraceShapeFunctionPointer, bool(*)(void*, const void*, const cs2::Vector*, const cs2::Vector*, void*, void*) noexcept);
STRONG_TYPE_ALIAS(GameTraceManagerStoragePointer, void**);
STRONG_TYPE_ALIAS(InitFilterFunctionPointer, void*(*)(void*, void*, std::uint64_t, std::uint8_t, std::uint8_t) noexcept);
STRONG_TYPE_ALIAS(CGameTraceEndPositionOffset, FieldOffset<engine_trace::OutputStorage, cs2::Vector, std::int32_t>);
STRONG_TYPE_ALIAS(CGameTraceNormalOffset, FieldOffset<engine_trace::OutputStorage, cs2::Vector, std::int32_t>);
STRONG_TYPE_ALIAS(CGameTraceFractionOffset, FieldOffset<engine_trace::OutputStorage, float, std::int32_t>);
