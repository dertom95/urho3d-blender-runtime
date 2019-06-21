//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "DummyComponent.h"

#include <Urho3D/Urho3DAll.h>


DummyComponent::DummyComponent(Context* context) :
    LogicComponent(context),
    boolValue_(DEFAULT_BOOL_VALUE),
    intValue_(DEFAULT_INT_VALUE),
    floatValue_(DEFAULT_FLOAT_VALUE),
    stringValue_(DEFAULT_STRING_VALUE)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_UPDATE);
}

void DummyComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<DummyComponent>("game component");

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
    URHO3D_ATTRIBUTE("BOOL VALUE", bool, boolValue_, DEFAULT_BOOL_VALUE, AM_FILE);
    URHO3D_ATTRIBUTE("INT VALUE", int, intValue_, DEFAULT_INT_VALUE, AM_FILE);
    URHO3D_ATTRIBUTE("FLOAT VALUE", float, floatValue_, DEFAULT_FLOAT_VALUE, AM_FILE);
    URHO3D_ATTRIBUTE("STRING VALUE", String, stringValue_, DEFAULT_STRING_VALUE, AM_FILE);
    //URHO3D_ATTRIBUTE("Anim Run", String, animRun_, "", AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Animation", GetAnimation, SetAnimation, ResourceRef, ResourceRef(Animation::GetTypeStatic()), AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Texture", GetTexture, SetTexture, ResourceRef, ResourceRef(Texture2D::GetTypeStatic()), AM_DEFAULT);
}

void DummyComponent::DelayedStart()
{
    // init whatever you want. at this point all nodes are already handled
    URHO3D_LOGINFO("STARTED");
    int a=0;
}

void DummyComponent::Update(float timeStep)
{
    // do the logic here

}

ResourceRef DummyComponent::GetAnimation() const
{
    return ResourceRef(Animation::GetTypeStatic(),animationName_);
}

void DummyComponent::SetAnimation(const ResourceRef &value)
{
    animationName_ = value.name_;
}

ResourceRef DummyComponent::GetTexture() const
{
    return ResourceRef(Texture::GetTypeStatic(),textureName_);
}

void DummyComponent::SetTexture(const ResourceRef &value)
{
    textureName_ = value.name_;
}
