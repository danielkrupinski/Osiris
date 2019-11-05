#pragma once

#include "Utils.h"

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE(MaterialLock_t);

class Material;
class RenderContext;

class MaterialSystem {
public:
    constexpr auto findMaterial(const char* materialName) noexcept
    {
        return callVirtualMethod<Material*, const char*, const char*, bool, const char*>(this, 84, materialName, nullptr, true, nullptr);
    }

	constexpr auto firstMaterial() noexcept
	{
		return callVirtualMethod<MaterialHandle_t>(this, 86);
	}

	constexpr auto nextMaterial(MaterialHandle_t h) noexcept
	{
		return callVirtualMethod<MaterialHandle_t, MaterialHandle_t>(this, 87, h);
	}

	constexpr auto invalidMaterial() noexcept
	{
		return callVirtualMethod<MaterialHandle_t>(this, 88);
	}

	constexpr auto getMaterial(MaterialHandle_t h) noexcept
	{
		return callVirtualMethod<Material*, MaterialHandle_t>(this, 89, h);
	}

    constexpr auto getRenderContext() noexcept
    {
        return callVirtualMethod<RenderContext*>(this, 115);
    }
};
