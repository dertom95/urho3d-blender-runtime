#include <Urho3D/Urho3DAll.h>

#include "NavigationMesh.h"
#include <Globals.h>

static const char* typeNames[] =
{
    "Dynamic",
    "Static",
    nullptr
};

static const NavMeshType DEFAULT_NAVMESHTYPE = NAV_Static;
static const int DEFAULT_TILESIZE=32;
static const float DEFAULT_AGENT_HEIGHT=0.5f;
static const float DEFAULT_AGENT_MAXCLIMB=0.25f;
static const float DEFAULT_CELL_HEIGHT=0.05;

CNavigationMesh::CNavigationMesh(Context* context)
    : LogicComponent(context)
      ,activateNavigation(false)
      ,showNavmesh(true)
      ,navmeshType(DEFAULT_NAVMESHTYPE)
      ,tileSize(DEFAULT_TILESIZE)
      ,agentHeight(DEFAULT_AGENT_HEIGHT)
      ,agentMaxClimb(DEFAULT_AGENT_MAXCLIMB)
      ,cellHeight(DEFAULT_CELL_HEIGHT)

{
    SetUpdateEventMask(USE_UPDATE);
}

void CNavigationMesh::RegisterObject(Context* context) {
    context->RegisterFactory<CNavigationMesh>();
    //URHO3D_ACCESSOR_ATTRIBUTE("Speed", GetSpeed, SetSpeed, float,1.5f,AM_DEFAULT);
    URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Navmesh Type",GetNavmeshType,SetNavmeshType,NavMeshType,typeNames,DEFAULT_NAVMESHTYPE,AM_DEFAULT);
    URHO3D_ATTRIBUTE("activateNavigation", bool, activateNavigation, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("showNavmesh", bool, showNavmesh, true, AM_DEFAULT);
    URHO3D_ATTRIBUTE("tileSize", int, tileSize, DEFAULT_TILESIZE, AM_DEFAULT);
    URHO3D_ATTRIBUTE("agent height", float, agentHeight, DEFAULT_AGENT_HEIGHT, AM_DEFAULT);
    URHO3D_ATTRIBUTE("agent max climb", float, agentMaxClimb, DEFAULT_AGENT_MAXCLIMB, AM_DEFAULT);
    URHO3D_ATTRIBUTE("cell height", float, cellHeight, DEFAULT_CELL_HEIGHT, AM_DEFAULT);
}

void CNavigationMesh::SetNavmeshType(NavMeshType type)
{
    navmeshType = type;
}

void CNavigationMesh::Start(){
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(CNavigationMesh, HandlePostRenderUpdate));
}


void CNavigationMesh::SetupNavigation()
{
    Scene* m_scene = Globals::instance()->scene;

    auto crowdManager_ = m_scene->CreateComponent<CrowdManager>();
    context_->RegisterSubsystem(crowdManager_);
    CrowdObstacleAvoidanceParams params = crowdManager_->GetObstacleAvoidanceParams(0);
    // Set the params to "High (66)" setting
    params.velBias = 0.5f;
    params.adaptiveDivs = 7;
    params.adaptiveRings = 3;
    params.adaptiveDepth = 3;
    crowdManager_->SetObstacleAvoidanceParams(0, params);

    // Create a DynamicNavigationMesh component to the scene root
    NavigationMesh* navMesh_ = navmeshType==NAV_Static?
                m_scene->CreateComponent<DynamicNavigationMesh>()
                :m_scene->CreateComponent<NavigationMesh>();
    // Set small tiles to show navigation mesh streaming
    navMesh_->SetTileSize(tileSize);

    navMesh_->SetDrawOffMeshConnections(true);
    // Set the agent height large enough to exclude the layers under boxes
    navMesh_->SetAgentHeight(agentHeight);
    navMesh_->SetAgentMaxClimb(agentMaxClimb);
    // Set nav mesh cell height to minimum (allows agents to be grounded)
    navMesh_->SetCellHeight(cellHeight);
    // Create a Navigable component to the scene root. This tags all of the geometry in the scene as being part of the
    // navigation mesh. By default this is recursive, but the recursion could be turned off from Navigable
    //m_scene->CreateComponent<Navigable>();
    // Add padding to the navigation mesh in Y-direction so that we can add objects on top of the tallest boxes
    // in the scene and still update the mesh correctly
    navMesh_->SetPadding(Vector3(0.0f, 10.0f, 0.0f));
    // Now build the navigation geometry. This will take some time. Note that the navigation mesh will prefer to use
    // physics geometry from the scene nodes, as it often is simpler, but if it can not find any (like in this example)
    // it will use renderable geometry instead

    navMesh_->SetAreaCost(1,2.0f);

    // Enable drawing debug geometry for obstacles and off-mesh connections
    if (navmeshType == NAV_Dynamic){
        DynamicNavigationMesh* dynNav = static_cast<DynamicNavigationMesh*>(navMesh_);
        dynNav->SetDrawObstacles(true);
    }
    navMesh_->Build();
    context_->RegisterSubsystem(navMesh_);
    crowdManager_->SetNavigationMesh(navMesh_);
/*
    // Create a NavigationMesh component to the scene root
    auto* navMesh = m_scene->CreateComponent<NavigationMesh>();
    context_->RegisterSubsystem(navMesh);
    // Set small tiles to show navigation mesh streaming
    navMesh->SetTileSize(32);
    // Create a Navigable component to the scene root. This tags all of the geometry in the scene as being part of the
    // navigation mesh. By default this is recursive, but the recursion could be turned off from Navigable
    m_scene->CreateComponent<Navigable>();
    // Add padding to the navigation mesh in Y-direction so that we can add objects on top of the tallest boxes
    // in the scene and still update the mesh correctly
    navMesh->SetPadding(Vector3(0.0f, 10.0f, 0.0f));
    // Now build the navigation geometry. This will take some time. Note that the navigation mesh will prefer to use
    // physics geometry from the scene nodes, as it often is simpler, but if it can not find any (like in this example)
    // it will use renderable geometry instead
    navMesh->Build();*/
}

void CNavigationMesh::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{

    if (!showNavmesh)
        return;

    DebugRenderer* dR = Globals::instance()->scene->GetComponent<DebugRenderer>();
    if (dR){
        DynamicNavigationMesh* navMesh_ = GetSubsystem<DynamicNavigationMesh>();
        navMesh_->DrawDebugGeometry(dR,true);

        dR->AddCircle(Vector3(0,0,0),Vector3::UP,2,Color::CYAN);
    }

}


void CNavigationMesh::Update(float timestep){
}

void CNavigationMesh::DelayedStart(){
    if (activateNavigation){
        SetupNavigation();
    }
}

