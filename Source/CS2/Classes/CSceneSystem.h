#pragma once

namespace cs2
{

struct CSceneObject;

struct CSceneSystem {
    using DeleteSceneObject = void(CSceneSystem* sceneSystem, CSceneObject* sceneObject);
    using AllocateAttributeList = void(CSceneSystem* sceneSystem, CSceneObject* sceneObject);
};

}
