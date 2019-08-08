#pragma once

#include <Urho3D/Core/Context.h>

#include <tools/SceneLoader/SampleComponents/PlayAnimation.h>
#include <tools/SceneLoader/SampleComponents/ShapeKey.h>
#include <tools/SceneLoader/SampleComponents/DummyComponent.h>
#include <tools/SceneLoader/SampleComponents/Rotator.h>



using namespace Urho3D;

class SampleComponents{
public:
    static void RegisterComponents(Context* context);
};

void SampleComponents::RegisterComponents(Context *context)
{
    PlayAnimation::RegisterObject(context);
    ShapeKey::RegisterObject(context);
    DummyComponent::RegisterObject(context);
    Rotator::RegisterObject(context);
}
