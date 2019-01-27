#pragma once

#include <tools/SceneLoader/LoaderTools/GroupInstance.h>
#include <tools/SceneLoader/SampleComponents/PlayAnimation.h>
#include <tools/SceneLoader/SampleComponents/Rotator.h>

class SceneLoaderComponents {
public:
    static void RegisterComponents(Context* context);
    static void RegisterSampleComponents(Context* context);
};

void SceneLoaderComponents::RegisterComponents(Context *context)
{
    // register group instance component
    GroupInstance::RegisterObject(context);
}

void SceneLoaderComponents::RegisterSampleComponents(Context *context)
{
    // register sample component for it will be exported
    Rotator::RegisterObject(context);
    PlayAnimation::RegisterObject(context);
}
