#include <Urho3D/Urho3DAll.h>
#include <Globals.h>

#include "Global.h"


static const float DEFAULT_ACTIVATE_NAVIGATION=false;
static const float DEFAULT_SHOW_NAVMESH=false;
static const float DEFAULT_SHOW_PHYSICS=false;

SceneLogic::SceneLogic(Context* context)
    : LogicComponent(context)
      ,activateNavigation(DEFAULT_ACTIVATE_NAVIGATION)
      ,showNavmesh(DEFAULT_SHOW_NAVMESH)
      ,showPhysics(DEFAULT_SHOW_PHYSICS)
      ,navMesh(0)

{
    SetUpdateEventMask(USE_UPDATE);
}

void SceneLogic::RegisterObject(Context* context) {
    context->RegisterFactory<SceneLogic>("game component");
    URHO3D_ATTRIBUTE("activateNavigation", bool, activateNavigation, DEFAULT_ACTIVATE_NAVIGATION, AM_DEFAULT);
    URHO3D_ATTRIBUTE("showNavmesh", bool, showNavmesh, DEFAULT_SHOW_NAVMESH, AM_DEFAULT);
    URHO3D_ATTRIBUTE("showPhysics", bool, showPhysics, DEFAULT_SHOW_PHYSICS, AM_DEFAULT);
}

void SceneLogic::Start(){
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(SceneLogic, HandlePostRenderUpdate));



}

void SceneLogic::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{
    if (showPhysics){
        PhysicsWorld* pw = Globals::instance()->scene->GetComponent<PhysicsWorld>();
        if (pw){
            pw->DrawDebugGeometry(false);
        }
    }

    if (!showNavmesh)
        return;

    DebugRenderer* dR = Globals::instance()->scene->GetComponent<DebugRenderer>();
    if (dR && navMesh){
        navMesh->DrawDebugGeometry(dR,true);
        dR->AddCircle(Vector3(0,0,0),Vector3::UP,2,Color::CYAN);
    }
}


void SceneLogic::Update(float timestep){
}

void SceneLogic::DelayedStart(){
    if (activateNavigation){
        SetupNavigation();
    }
}

void SceneLogic::SetupNavigation()
{
    navMesh = Globals::instance()->scene->GetComponent<NavigationMesh>();

    if (navMesh){
        navMesh->Build();
    }
    else {
        // TODO: unify this? or make it sense cause we might need different logic for dynamicmeshes?
        navMesh = Globals::instance()->scene->GetComponent<DynamicNavigationMesh>();
        if (navMesh){
            navMesh->Build();
        }
    }
}
