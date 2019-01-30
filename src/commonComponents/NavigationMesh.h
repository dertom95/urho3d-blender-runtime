#pragma once

enum NavMeshType {
    NAV_Dynamic,NAV_Static
};

class CNavigationMesh : public LogicComponent
{
  URHO3D_OBJECT(CNavigationMesh,LogicComponent);

public:
    explicit CNavigationMesh(Context* context);

    static void RegisterObject(Context* context);

    void Start() override;

    void DelayedStart() override;
    void Update(float timestep) override;
    void HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData);

    NavMeshType GetNavmeshType() const { return navmeshType;}
    void SetNavmeshType(NavMeshType type);
private:
    void SetupNavigation();

    NavMeshType navmeshType;
    bool activateNavigation;
    bool showNavmesh;

    float agentHeight;
    float agentMaxClimb;
    float cellHeight;
    int tileSize;

};
