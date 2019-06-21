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

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/AnimationController.h>

using namespace Urho3D;

const bool DEFAULT_BOOL_VALUE = true;
const float DEFAULT_FLOAT_VALUE = 18.95f;
const int DEFAULT_INT_VALUE = 95;
const String DEFAULT_STRING_VALUE="FORTUNA";

/// DummyComponent component, responsible for physical movement according to controls, as well as animation.
class DummyComponent : public LogicComponent
{
    URHO3D_OBJECT(DummyComponent, LogicComponent);

public:
    /// Construct.
    explicit DummyComponent(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    void DelayedStart() override;

    /// Handle physics world update. Called by LogicComponent base class.
    void Update(float timeStep) override;

    void SetAnimation(const ResourceRef& value);
    ResourceRef GetAnimation() const;

    void SetTexture(const ResourceRef& value);
    ResourceRef GetTexture() const;

private:
    bool boolValue_;
    int intValue_;
    float floatValue_;
    String stringValue_;
    Vector3 vec3Value_;

    String animationName_;
    String textureName_;
};
