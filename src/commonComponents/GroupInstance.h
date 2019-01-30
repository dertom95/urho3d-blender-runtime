#pragma once

#include <Urho3D/Urho3DAll.h>

class GroupInstance : public Component
{
    URHO3D_OBJECT(GroupInstance,Component);
public:
    static void RegisterObject(Context *context);

    GroupInstance(Context* ctx);

    void SetGroupFilename(const String& grpInstanceName);
    const String& GetGroupFilename() const  { return groupFilename;}

private:
    String groupFilename;

};


