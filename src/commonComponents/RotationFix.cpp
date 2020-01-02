#include "RotationFix.h"

RotationFix::RotationFix(Context *ctx)
    : Component(ctx)
{}

void RotationFix::RegisterObject(Context *context)
{
    context->RegisterFactory<RotationFix>();
}

void RotationFix::OnSceneSet(Scene *scene)
{
    node_->Rotate(Quaternion(90,0,90));
}

