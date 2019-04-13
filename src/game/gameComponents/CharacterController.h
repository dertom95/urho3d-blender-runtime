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

class Character;
class Touch;

using namespace Urho3D;

const float DEFAULT_CAMERA_MIN_DIST = 1.0f;
const float DEFAULT_CAMERA_INITIAL_DIST = 5.0f;
const float DEFAULT_CAMERA_MAX_DIST = 20.0f;

const float DEFAULT_YAW_SENSITIVITY = 0.1f;


/// CharacterController component, responsible for physical movement according to controls, as well as animation.
class CharacterController : public LogicComponent
{
    URHO3D_OBJECT(CharacterController, LogicComponent);

public:
    /// Construct.
    explicit CharacterController(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    void Start() override;

    void DelayedStart() override;

    /// Handle application update. Set controls to character.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle application post-update. Update camera position after character has moved.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);


private:
    /// The controllable character component.
    WeakPtr<Character> character_;

    Node* cameraNode_;
    Camera* camera_;

    bool m_useCamera;

    bool m_firstPerson;

    float CAMERA_MIN_DIST;
    float CAMERA_INITIAL_DIST;
    float CAMERA_MAX_DIST;

    float YAW_SENSITIVITY;

    /// Touch utility object.
    //SharedPtr<Touch> touch_;

};
