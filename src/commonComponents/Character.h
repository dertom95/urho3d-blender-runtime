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


const unsigned CTRL_FORWARD = 1;
const unsigned CTRL_BACK = 2;
const unsigned CTRL_LEFT = 4;
const unsigned CTRL_RIGHT = 8;
const unsigned CTRL_JUMP = 16;

const float DEFAULT_MOVE_FORCE = 0.8f;
const float DEFAULT_INAIR_MOVE_FORCE = 0.02f;
const float DEFAULT_BRAKE_FORCE = 0.2f;
const float DEFAULT_JUMP_FORCE = 7.0f;
const float DEFAULT_INAIR_THRESHOLD_TIME = 0.1f;

/// Character component, responsible for physical movement according to controls, as well as animation.
class Character : public LogicComponent
{
    URHO3D_OBJECT(Character, LogicComponent);

public:
    /// Construct.
    explicit Character(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    void DelayedStart() override;

    /// Handle physics world update. Called by LogicComponent base class.
    void FixedUpdate(float timeStep) override;

    /// Movement controls. Assigned by the main program each frame.
    Controls controls_;

    void SetAnimationRun(const ResourceRef& value);
    ResourceRef GetAnimationRun() const;
    void SetAnimationIdle(const ResourceRef& value);
    ResourceRef GetAnimationIdle() const;
    void SetAnimationJump(const ResourceRef& value);
    ResourceRef GetAnimationJump() const;

    inline Node* GetHeadNode() { return headNode_; }
private:
    /// Handle physics collision event.
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);

    /// Grounded flag for movement.
    bool onGround_;
    /// Jump flag.
    bool okToJump_;
    /// In air timer. Due to possible physics inaccuracy, character can be off ground for max. 1/10 second and still be allowed to move.
    float inAirTimer_;

    Node* visualNode_;
    AnimationController* animCtrl_;
    AnimatedModel* animModel_;

    String headNodeName_;

    Node* headNode_;

    String animRun_;
    String animJump_;
    String animIdle_;

    float MOVE_FORCE;
    float INAIR_MOVE_FORCE;
    float BRAKE_FORCE;
    float JUMP_FORCE;
    float INAIR_THRESHOLD_TIME;

};
