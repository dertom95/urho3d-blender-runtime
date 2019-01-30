#include "GroupInstance.h"

GroupInstance::GroupInstance(Context *ctx)
    : Component(ctx)
{}

void GroupInstance::RegisterObject(Context *context)
{
    context->RegisterFactory<GroupInstance>();

    URHO3D_ACCESSOR_ATTRIBUTE("groupFilename", GetGroupFilename, SetGroupFilename, String, String::EMPTY, AM_DEFAULT);
}

void GroupInstance::SetGroupFilename(const String &groupFilename)
{
    if (groupFilename == this->groupFilename) return;

    this->groupFilename=groupFilename;

    FileSystem* fs = GetSubsystem<FileSystem>();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* file = cache->GetResource<XMLFile>(groupFilename);
    Node* groupRoot = node_->CreateChild("group_root");
    groupRoot->LoadXML(file->GetRoot());
}
