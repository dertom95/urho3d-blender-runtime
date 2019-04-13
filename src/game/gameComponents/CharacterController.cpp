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
#include <Urho3D/Urho3DAll.h>

#include "CharacterController.h"
#include "Character.h"
#include "Globals.h"

CharacterController::CharacterController(Context* context) :
    LogicComponent(context),
    m_useCamera(true),
    m_firstPerson(false),
    CAMERA_MIN_DIST(DEFAULT_CAMERA_MIN_DIST),
    CAMERA_INITIAL_DIST(DEFAULT_CAMERA_INITIAL_DIST),
    CAMERA_MAX_DIST(DEFAULT_CAMERA_MAX_DIST),
    YAW_SENSITIVITY(DEFAULT_YAW_SENSITIVITY)

{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

void CharacterController::RegisterObject(Context* context)
{
    context->RegisterFactory<CharacterController>("game component");

    URHO3D_ATTRIBUTE("Use Camera", bool, m_useCamera, true, AM_DEFAULT);

    URHO3D_ATTRIBUTE("First Person", bool, m_firstPerson, false, AM_DEFAULT);

    URHO3D_ATTRIBUTE("CAMERA_MIN_DIST", float, CAMERA_MIN_DIST, DEFAULT_CAMERA_MIN_DIST, AM_DEFAULT);
    URHO3D_ATTRIBUTE("CAMERA_INITIAL_DIST", float, CAMERA_INITIAL_DIST, DEFAULT_CAMERA_INITIAL_DIST, AM_DEFAULT);
    URHO3D_ATTRIBUTE("CAMERA_MAX_DIST", float, CAMERA_MAX_DIST, DEFAULT_CAMERA_MAX_DIST, AM_DEFAULT);
    URHO3D_ATTRIBUTE("YAW_SENSITIVITY", float, YAW_SENSITIVITY, DEFAULT_YAW_SENSITIVITY, AM_DEFAULT);

}

void CharacterController::Start()
{
    // Subscribe to Update event for setting the character controls before physics simulation
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CharacterController, HandleUpdate));

    // Subscribe to PostUpdate event for updating the camera position after physics simulation
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(CharacterController, HandlePostUpdate));

    // Unsubscribe the SceneUpdate event from base class as the camera node is being controlled in HandlePostUpdate() in this sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

void CharacterController::DelayedStart()
{
    character_ = node_->GetComponent<Character>(true);

    if (!character_){
        URHO3D_LOGERROR("COULD NOT FIND CHILD WITH COMPONENT 'Character'");
        node_->RemoveComponent(this);
    }

    if (m_useCamera){
        cameraNode_ = node_->GetScene()->CreateChild("character_cam");
        camera_ = cameraNode_->CreateComponent<Camera>();
    }
}

void CharacterController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    auto* input = GetSubsystem<Input>();

    if (character_)
    {
        // Clear previous controls
        character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);

        // Update controls using touch utility class
//        if (touch_)
//            touch_->UpdateTouches(character_->controls_);

        // Update controls using keys
        auto* ui = GetSubsystem<UI>();
        if (!ui->GetFocusElement())
        {
            //if (!touch_ || !touch_->useGyroscope_)
            {
                character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_UP));
                character_->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_DOWN));
                character_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_LEFT));
                character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_RIGHT));
            }
            character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

            // Add character yaw & pitch from the mouse motion or touch input
            /*if (touchEnabled_)
            {
                for (unsigned i = 0; i < input->GetNumTouches(); ++i)
                {
                    TouchState* state = input->GetTouch(i);
                    if (!state->touchedElement_)    // Touch on empty space
                    {
                        auto* camera = cameraNode_->GetComponent<Camera>();
                        if (!camera)
                            return;

                        auto* graphics = GetSubsystem<Graphics>();
                        character_->controls_.yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
                        character_->controls_.pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;
                    }
                }
            }
            else
            {*/
                character_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
                character_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
            //}

            // Limit pitch
            character_->controls_.pitch_ = Clamp(character_->controls_.pitch_, -80.0f, 80.0f);
            // Set rotation already here so that it's updated every rendering frame instead of every physics frame
            character_->GetNode()->SetRotation(Quaternion(character_->controls_.yaw_, Vector3::UP));

//            // Switch between 1st and 3rd person
//            if (input->GetKeyPress(KEY_F))
//                firstPerson_ = !firstPerson_;

            // Turn on/off gyroscope on mobile platform
//            if (touch_ && input->GetKeyPress(KEY_G))
//                touch_->useGyroscope_ = !touch_->useGyroscope_;

            // Check for loading / saving the scene
//            if (input->GetKeyPress(KEY_F5))
//            {
//                File saveFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/CharacterDemo.xml", FILE_WRITE);
//                scene_->SaveXML(saveFile);
//            }
//            if (input->GetKeyPress(KEY_F7))
//            {
//                File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/CharacterDemo.xml", FILE_READ);
//                scene_->LoadXML(loadFile);
//                // After loading we have to reacquire the weak pointer to the Character component, as it has been recreated
//                // Simply find the character's scene node by name as there's only one of them
//                Node* characterNode = scene_->GetChild("Jack", true);
//                if (characterNode)
//                    character_ = characterNode->GetComponent<Character>();
//            }
        }
    }
}

void CharacterController::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!character_)
        return;

    Node* characterNode = character_->GetNode();

    // Get camera lookat dir from character yaw + pitch
    const Quaternion& rot = characterNode->GetRotation();
    Quaternion dir = rot * Quaternion(character_->controls_.pitch_, Vector3::RIGHT);

    // Turn head to camera pitch, but limit to avoid unnatural animation

    if (m_firstPerson && character_->GetHeadNode())
    {
        cameraNode_->SetPosition(character_->GetHeadNode()->GetWorldPosition() + rot * Vector3(0.0f, 0.15f, 0.2f));
        cameraNode_->SetRotation(dir);
    }
    else
     {
        if (cameraNode_){
            // Third person camera: position behind the character
            Vector3 aimPoint = characterNode->GetPosition() + rot * Vector3(0.0f, 3.7f, 0.0f);

            // Collide camera ray with static physics objects (layer bitmask 2) to ensure we see the character properly
            Vector3 rayDir = dir * Vector3::BACK;
    //        float rayDistance = touch_ ? touch_->cameraDistance_ : CAMERA_INITIAL_DIST;
            float rayDistance = CAMERA_INITIAL_DIST;
            PhysicsRaycastResult result;
            node_->GetScene()->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);
            if (result.body_)
                rayDistance = Min(rayDistance, result.distance_);
            rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);


            cameraNode_->SetPosition(aimPoint + rayDir * rayDistance);
            cameraNode_->SetRotation(dir);
        }
    }
}


