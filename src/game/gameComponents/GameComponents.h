#pragma once

#include <Urho3D/Core/Context.h>

#include "Global.h"

using namespace Urho3D;

class GameComponents{
public:
    static void RegisterComponents(Context* context);
};

void GameComponents::RegisterComponents(Context *context)
{
    SceneLogic::RegisterObject(context);
}
