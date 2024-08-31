#pragma once

namespace cs2
{

struct CSceneObject;

struct CSceneSystem {
    using DeleteSceneObject = void(CSceneSystem* sceneSystem, CSceneObject* sceneObject);
};

}
