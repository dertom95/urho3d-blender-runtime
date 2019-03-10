#include "ShapeKey.h"
#include <Urho3D/Urho3DAll.h>

ShapeKey::ShapeKey(Context *ctx)
    : LogicComponent(ctx),value(0.0f)
{}

void ShapeKey::RegisterObject(Context *context)
{
    context->RegisterFactory<ShapeKey>();

    URHO3D_ATTRIBUTE("shapeKey", String, shapeKey, "", AM_DEFAULT);
    URHO3D_ATTRIBUTE("value", float, value, 0.0f, AM_DEFAULT);
}


void ShapeKey::DelayedStart()
{
    AnimatedModel* model = node_->GetComponent<AnimatedModel>();
    model->SetMorphWeight(shapeKey,value);
}
