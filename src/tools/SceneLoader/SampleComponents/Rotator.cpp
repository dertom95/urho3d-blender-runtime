#include "Rotator.h"

Rotator::Rotator(Context* context)
    : LogicComponent(context),
      speed(8),
      axis(Vector3(1,1,1))
{
    SetUpdateEventMask(USE_UPDATE);
}

void Rotator::RegisterObject(Context* context) {
    context->RegisterFactory<Rotator>("Sample Component");
    //URHO3D_ACCESSOR_ATTRIBUTE("Speed", GetSpeed, SetSpeed, float,1.5f,AM_DEFAULT);
    URHO3D_ATTRIBUTE("axis", Vector3, axis, Vector3(1,1,1), AM_DEFAULT);
    URHO3D_ATTRIBUTE("speed", float, speed, 8.0f, AM_DEFAULT);
}

void Rotator::Start(){
}





void Rotator::Update(float timestep){
    //node_-
    //rotateAxis = axis * timestep * speed;
    node_->Roll(timestep * speed);
   //URHO3D_LOGINFOF("ROTATE:%s", rotateAxis.ToString().CString());
}

