//
// Copyright (c) 2008-2018 the Urho3D project.
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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/DebugNew.h>
#include <Urho3D/Resource/XMLFile.h>

#include "SceneLoader.h"

#include "LoaderTools/ComponentExporter.h"
#include "LoaderTools/GroupInstance.h"

#include "SampleComponents/PlayAnimation.h"
#include "SampleComponents/Rotator.h"
#include "SceneLoaderComponents.h"


URHO3D_DEFINE_APPLICATION_MAIN(SceneLoader)

SceneLoader::SceneLoader(Context* context) :
    Sample(context), sceneName("Scene.xml")
{
    // register component exporter
    context->RegisterSubsystem(new Urho3DNodeTreeExporter(context));
    // register group instance component
    SceneLoaderComponents::RegisterComponents(context);
    SceneLoaderComponents::RegisterSampleComponents(context);
}


void SceneLoader::Start()
{
    auto args = GetArguments();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    for (int i=0;i < args.Size(); i++){
        if (args[i]=="--workpath" && (i+1)<args.Size()){
            String workpath = args[i+1];
            cache->AddResourceDir(workpath,0);
            i++;
        }
        else if (args[i]=="--scenename" && (i+1)<args.Size()){
            sceneName = args[i+1];
            i++;
        }
        else if (args[i]=="--runtimeflags" && (i+1)<args.Size()){
            String flags(args[i+1]);
            runtimeFlags = flags.Split(';');
            i++;
        }
    }
    // Execute base class startup
    Sample::Start();

    // Create the scene content
    bool foundScene = CreateScene();
    if (!foundScene)
        return;

    // Create the UI content
    CreateUI();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Subscribe to global events for camera movement
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);

    ExportComponents("./urho3d_components.json");
}

void SceneLoader::ExportComponents(const String& outputPath)
{
    Urho3DNodeTreeExporter* exporter = GetSubsystem<Urho3DNodeTreeExporter>();
    // set whitelist-mode to tell the exporter what components to include for export
    exporter->SetExportMode(Urho3DNodeTreeExporter::WhiteList);

    // include all Components that inherit from LogicComponent
    exporter->AddSuperComponentHashToFilterList(LogicComponent::GetTypeStatic());

    exporter->AddComponentHashToFilterList(Light::GetTypeStatic());
    exporter->AddComponentHashToFilterList(RigidBody::GetTypeStatic());
    exporter->AddComponentHashToFilterList(CollisionShape::GetTypeStatic());
    exporter->AddComponentHashToFilterList(Navigable::GetTypeStatic());
    exporter->AddComponentHashToFilterList(NavArea::GetTypeStatic());
    exporter->AddComponentHashToFilterList(GroupInstance::GetTypeStatic());
    exporter->AddComponentHashToFilterList(PlayAnimation::GetTypeStatic());

    exporter->AddCustomUIFile("./customui.py");
    exporter->AddMaterialFolder("Materials");
    exporter->AddTechniqueFolder("Techniques");
    exporter->Export(outputPath);
}

bool SceneLoader::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);

    SharedPtr<File> file = cache->GetFile("Scenes/"+sceneName);
    if (file.Null()){
        URHO3D_LOGERROR("SceneLoader could not find 'Scenes/Scene.xml' in its resource-path");
        engine_->Exit();
        return false;
    }
    cache->SetAutoReloadResources(true);
    scene_->LoadXML(*file);

    // Create the camera (not included in the scene file)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();
    cameraNode_->SetPosition(Vector3(0.0f, 2.0f, -10.0f));

/*
    // set a light
    // TODO: Use only the light provided by scene (once this is exported correctly)
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    auto* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);*/

    return true;
}

void SceneLoader::ReloadScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> file = cache->GetFile("Scenes/Scene.xml");
    if (file.Null()){
        URHO3D_LOGERROR("SceneLoader could not find 'Scenes/Scene.xml' in its resource-path");
        engine_->Exit();
    }
    scene_->LoadXML(*file);

    // set a light
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    auto* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    File saveFile(context_, "./scene.write.xml",FILE_WRITE);
    scene_->SaveXML(saveFile);
}

void SceneLoader::CreateUI()
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* ui = GetSubsystem<UI>();

    // Set up global UI style into the root UI element
    auto* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    ui->GetRoot()->SetDefaultStyle(style);

    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it will interact with the UI
    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto();
    ui->SetCursor(cursor);
    // Set starting position of the cursor at the rendering window center
    auto* graphics = GetSubsystem<Graphics>();
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);

    // Load UI content prepared in the editor and add to the UI hierarchy
}

void SceneLoader::SetupViewport()
{
    auto* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void SceneLoader::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for camera motion
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SceneLoader, HandleUpdate));
    using namespace FileChanged;
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(SceneLoader, HandleFileChanged));
}

void SceneLoader::MoveCamera(float timeStep)
{
    // Right mouse button controls mouse cursor visibility: hide when pressed
    auto* ui = GetSubsystem<UI>();
    auto* input = GetSubsystem<Input>();
    ui->GetCursor()->SetVisible(!input->GetMouseButtonDown(MOUSEB_RIGHT));

    // Do not move if the UI has a focused element
    if (ui->GetFocusElement())
        return;

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    // Only move the camera when the cursor is hidden
    if (!ui->GetCursor()->IsVisible())
    {
        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);

        // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    }

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void SceneLoader::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;
    String filename = eventData[P_FILENAME].GetString();
    String resName = eventData[P_RESOURCENAME].GetString();

    if (resName=="Scenes/Scene.xml"){
        ReloadScene();
    }

    URHO3D_LOGINFOF("File Changed: fn:%s resname:%s",filename.CString(),resName.CString());
}

void SceneLoader::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);
}


