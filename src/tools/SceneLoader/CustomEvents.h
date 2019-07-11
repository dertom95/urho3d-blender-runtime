#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_BLENDER_MSG, BlenderConnect)
{
    URHO3D_PARAM(P_TOPIC, Channel);  //int
    URHO3D_PARAM(P_DATA, Sender);  //string
}


