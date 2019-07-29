#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_BLENDER_MSG, BlenderConnect)
{
    URHO3D_PARAM(P_TOPIC, Topic);  //string
    URHO3D_PARAM(P_SUBTYPE, SubType); // string
    URHO3D_PARAM(P_DATATYPE, DataType); // string
    URHO3D_PARAM(P_DATA, Data);  //Custom
}


