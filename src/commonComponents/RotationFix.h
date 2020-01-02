#pragma once

#include <Urho3D/Urho3DAll.h>

class RotationFix : public Component
{
    URHO3D_OBJECT(RotationFix,Component);
public:
    static void RegisterObject(Context *context);

    RotationFix(Context* ctx);

    void OnSceneSet(Scene* scene) override;

};
