#pragma once

#include <Urho3D/Core/Context.h>

#include <commonComponents/GroupInstance.h>
#include <commonComponents/NavigationMesh.h>
#include <commonComponents/ParentBone.h>
#include <commonComponents/Character.h>
#include <commonComponents/CharacterController.h>


using namespace Urho3D;

class CommonComponents{
public:
    static void RegisterComponents(Context* context);
};

void CommonComponents::RegisterComponents(Context *context)
{
    GroupInstance::RegisterObject(context);
    CNavigationMesh::RegisterObject(context);
    ParentBone::RegisterObject(context);
}
