#pragma once

#include <Urho3D/Core/Context.h>

#include <commonComponents/GroupInstance.h>
#include <commonComponents/NavigationMesh.h>
#include <tools/SceneLoader/SampleComponents/Rotator.h>
#include <tools/SceneLoader/SampleComponents/PlayAnimation.h>
#include <tools/SceneLoader/SampleComponents/ShapeKey.h>
#include <tools/SceneLoader/SampleComponents/ParentBone.h>


using namespace Urho3D;

class CommonComponents{
public:
    static void RegisterComponents(Context* context);
};

void CommonComponents::RegisterComponents(Context *context)
{
    GroupInstance::RegisterObject(context);
    CNavigationMesh::RegisterObject(context);
    Rotator::RegisterObject(context);
    PlayAnimation::RegisterObject(context);
    ShapeKey::RegisterObject(context);
    ParentBone::RegisterObject(context);

}
