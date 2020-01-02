#include "CameraFix.h"

CameraFix::CameraFix(Context *ctx)
    : Component(ctx)
{}

void CameraFix::RegisterObject(Context *context)
{
    context->RegisterFactory<CameraFix>();
}

void CameraFix::OnSceneSet(Scene *scene)
{
    node_->Rotate(Quaternion(90,0,90));
}
