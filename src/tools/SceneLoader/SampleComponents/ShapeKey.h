#pragma once

#include <Urho3D/Urho3DAll.h>

class ShapeKey : public LogicComponent
{
    URHO3D_OBJECT(ShapeKey,LogicComponent);
public:
    static void RegisterObject(Context *context);

    ShapeKey(Context* ctx);

    void SetShapeKey(const String& shapeKey,float value);
    virtual void DelayedStart() override;

private:
    String shapeKey;
    float value;
};


