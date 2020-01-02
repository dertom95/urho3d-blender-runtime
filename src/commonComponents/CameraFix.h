#pragma once

#include <Urho3D/Urho3DAll.h>

class CameraFix : public Component
{
    URHO3D_OBJECT(CameraFix,Component);
public:
    static void RegisterObject(Context *context);

    CameraFix(Context* ctx);

    void OnSceneSet(Scene* scene) override;

};


