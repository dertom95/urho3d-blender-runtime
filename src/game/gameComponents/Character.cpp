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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Animation.h>

#include <Urho3D/Urho3DAll.h>

#include "Character.h"

Character::Character(Context* context) :
    LogicComponent(context),
    onGround_(false),
    okToJump_(true),
    inAirTimer_(0.0f),
    MOVE_FORCE(DEFAULT_MOVE_FORCE),
    INAIR_MOVE_FORCE(DEFAULT_INAIR_MOVE_FORCE),
    BRAKE_FORCE(DEFAULT_BRAKE_FORCE),
    JUMP_FORCE(DEFAULT_JUMP_FORCE),
    INAIR_THRESHOLD_TIME(DEFAULT_INAIR_THRESHOLD_TIME)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

void Character::RegisterObject(Context* context)
{
    context->RegisterFactory<Character>("game component");

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_FILE | AM_NOEDIT);
    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_FILE | AM_NOEDIT);
    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_FILE |AM_NOEDIT);
    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_FILE | AM_NOEDIT);
    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_FILE | AM_NOEDIT);
    //URHO3D_ATTRIBUTE("Anim Run", String, animRun_, "", AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Animation Idle", GetAnimationIdle, SetAnimationIdle, ResourceRef, ResourceRef(Animation::GetTypeStatic()), AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Animation Run", GetAnimationRun, SetAnimationRun, ResourceRef, ResourceRef(Animation::GetTypeStatic()), AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Animation Jump", GetAnimationJump, SetAnimationJump, ResourceRef, ResourceRef(Animation::GetTypeStatic()), AM_DEFAULT);
    URHO3D_ATTRIBUTE("Head Bone", String, headNodeName_, "", AM_DEFAULT);

    URHO3D_ATTRIBUTE("MOVE_FORCE", float, MOVE_FORCE, DEFAULT_MOVE_FORCE, AM_DEFAULT);
    URHO3D_ATTRIBUTE("INAIR_MOVE_FORCE", float, INAIR_MOVE_FORCE, DEFAULT_INAIR_MOVE_FORCE, AM_DEFAULT);
    URHO3D_ATTRIBUTE("JUMP_FORCE", float, JUMP_FORCE, DEFAULT_JUMP_FORCE, AM_DEFAULT);
    URHO3D_ATTRIBUTE("INAIR_THRESHOLD_TIME", float, INAIR_THRESHOLD_TIME, DEFAULT_INAIR_THRESHOLD_TIME, AM_DEFAULT);
    URHO3D_ATTRIBUTE("BRAKE_FORCE", float, BRAKE_FORCE, DEFAULT_BRAKE_FORCE, AM_DEFAULT);
}

void Character::DelayedStart()
{
    animCtrl_ = node_->GetComponent<AnimationController>(true);
    animModel_ = node_->GetComponent<AnimatedModel>(true);
    animModel_->SetCastShadows(true);
    if (animCtrl_ && animModel_ && !headNodeName_.Empty()){
        Skeleton& skeleton = animModel_->GetSkeleton();
        Bone* bone = skeleton.GetBone(headNodeName_);
        if (bone){
            bone->animated_ = false;
            headNode_ = node_->GetChild(headNodeName_,true);
        }
    }

    // Component has been inserted into its scene node. Subscribe to events now
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Character, HandlePostUpdate));

}

void Character::FixedUpdate(float timeStep)
{
    /// \todo Could cache the components for faster access instead of finding them each frame
    auto* body = GetComponent<RigidBody>();
    AnimationController* animCtrl = node_->GetComponent<AnimationController>(true);

    // Update the in air timer. Reset if grounded
    if (!onGround_)
        inAirTimer_ += timeStep;
    else
        inAirTimer_ = 0.0f;
    // When character has been in air less than 1/10 second, it's still interpreted as being on ground
    bool softGrounded = inAirTimer_ < INAIR_THRESHOLD_TIME;

    // Update movement & animation
    const Quaternion& rot = node_->GetRotation();
    Vector3 moveDir = Vector3::ZERO;
    const Vector3& velocity = body->GetLinearVelocity();
    // Velocity on the XZ plane
    Vector3 planeVelocity(velocity.x_, 0.0f, velocity.z_);

    if (controls_.IsDown(CTRL_FORWARD))
        moveDir += Vector3::FORWARD;
    if (controls_.IsDown(CTRL_BACK))
        moveDir += Vector3::BACK;
    if (controls_.IsDown(CTRL_LEFT))
        moveDir += Vector3::LEFT;
    if (controls_.IsDown(CTRL_RIGHT))
        moveDir += Vector3::RIGHT;

    // Normalize move vector so that diagonal strafing is not faster
    if (moveDir.LengthSquared() > 0.0f)
        moveDir.Normalize();

    // If in air, allow control, but slower than when on ground
    body->ApplyImpulse(rot * moveDir * (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

    if (softGrounded)
    {
        // When on ground, apply a braking force to limit maximum ground velocity
        Vector3 brakeForce = -planeVelocity * BRAKE_FORCE;
        body->ApplyImpulse(brakeForce);

        // Jump. Must release jump control between jumps
        if (controls_.IsDown(CTRL_JUMP))
        {
            if (okToJump_)
            {
                body->ApplyImpulse(Vector3::UP * JUMP_FORCE);
                okToJump_ = false;
                animCtrl->PlayExclusive(animJump_, 0, false, 0.2f);
            }
        }
        else
            okToJump_ = true;
    }

    if ( !onGround_ )
    {
        animCtrl->PlayExclusive(animJump_, 0, false, 0.2f);
    }
    else
    {
        // Play walk animation if moving on ground, otherwise fade it out
        if (softGrounded && !moveDir.Equals(Vector3::ZERO))
            animCtrl->PlayExclusive(animRun_, 0, true, 0.2f);
        else
            animCtrl->PlayExclusive(animIdle_, 0, true, 0.2f);

        // Set walk animation speed proportional to velocity
        animCtrl->SetSpeed(animIdle_, planeVelocity.Length() * 0.3f);
    }

    // Reset grounded flag for next frame
    onGround_ = false;
}

void Character::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!headNode_)
        return;

    // Get camera lookat dir from character yaw + pitch
    const Quaternion& rot = node_->GetRotation();
    Quaternion dir = rot * Quaternion(controls_.pitch_, Vector3::RIGHT);

    // Turn head to camera pitch, but limit to avoid unnatural animation
    float limitPitch = Clamp(controls_.pitch_, -45.0f, 45.0f);
    Quaternion headDir = rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));
    // This could be expanded to look at an arbitrary target, now just look at a point in front
    Vector3 headWorldTarget = headNode_->GetWorldPosition() + headDir * Vector3(0.0f, 0.0f, -1.0f);
    headNode_->LookAt(headWorldTarget, Vector3(0.0f, 1.0f, 0.0f));
}

void Character::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
    using namespace NodeCollision;

    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    while (!contacts.IsEof())
    {
        Vector3 contactPosition = contacts.ReadVector3();
        Vector3 contactNormal = contacts.ReadVector3();
        /*float contactDistance = */contacts.ReadFloat();
        /*float contactImpulse = */contacts.ReadFloat();

        // If contact is below node center and pointing up, assume it's a ground contact
        if (contactPosition.y_ < (node_->GetWorldPosition().y_ + 1.0f))
        {
            float level = contactNormal.y_;
            if (level > 0.75)
                onGround_ = true;
        }
    }
}



ResourceRef Character::GetAnimationRun() const
{
    return ResourceRef(Animation::GetTypeStatic(),animRun_);
}

void Character::SetAnimationRun(const ResourceRef &value)
{
    animRun_ = value.name_;
}

ResourceRef Character::GetAnimationIdle() const
{
    return ResourceRef(Animation::GetTypeStatic(),animIdle_);
}
void Character::SetAnimationIdle(const ResourceRef &value)
{
    animIdle_ = value.name_;
}

ResourceRef Character::GetAnimationJump() const
{
    return ResourceRef(Animation::GetTypeStatic(),animJump_);
}
void Character::SetAnimationJump(const ResourceRef &value)
{
    animJump_ = value.name_;
}

