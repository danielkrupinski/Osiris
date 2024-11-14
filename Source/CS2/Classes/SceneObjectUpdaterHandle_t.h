#pragma once

#include <cstdint>

namespace cs2
{

struct CSceneObject;

struct SceneObjectUpdaterHandle_t {
   using sceneObject = CSceneObject*;

   void* updaterFunctionParameter;
   std::uint64_t(*updaterFunction)(void* parameter, void* unknown, bool unknownBool);
};

}
