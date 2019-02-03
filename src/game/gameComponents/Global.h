#pragma once

#include "Urho3D/Urho3DAll.h"
#include "Global.h"

class SceneLogic : public LogicComponent
{
  URHO3D_OBJECT(SceneLogic,LogicComponent);

public:
    explicit SceneLogic(Context* context);

    static void RegisterObject(Context* context);

    void Start() override;

    void DelayedStart() override;
    void Update(float timestep) override;
    void HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData);

private:
    void SetupNavigation();

    bool activateNavigation;
    bool showNavmesh;

    NavigationMesh* navMesh;

};

