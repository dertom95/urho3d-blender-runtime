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

#pragma once

#include <Sample.h>
#include<Urho3D/AngelScript/Script.h>
#include<Urho3D/Urho3DAll.h>

namespace Urho3D
{

class Drawable;
class Node;
class Scene;

}

class ViewRenderer{
public:
    ViewRenderer(Context* ctx,int id, Scene* initialScene, int width,int height,float fov);
    void SetSize(int width,int height,float fov);
    void SetScene(Scene* scene);
    void SetViewMatrix(const Matrix4& vmat);
    void SetViewMatrix(const Vector3& t,const Vector3& r,const Vector3& s);
    inline SharedPtr<Texture2D> GetRenderTexture(){ return renderTexture_;}
    inline int GetId() { return viewId_;}
    inline SharedPtr<Scene> GetScene() { return currentScene_; }
    inline SharedPtr<Camera> GetCamera() { return viewportCamera_;}
    const String& GetNetId() { return netId; }
    void RequestRender();
    void Show();
        float fov_;
private:

    String netId;
    int viewId_;
    int width_;
    int height_;

    Context* ctx_;
    SharedPtr<Scene> currentScene_;
    SharedPtr<RenderSurface> renderSurface_;
    SharedPtr<Texture2D> renderTexture_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<Node> viewportCameraNode_;
    SharedPtr<Camera> viewportCamera_;
};

/// Scene & UI load example.
/// This sample demonstrates:
///      - Loading a scene from a file and showing it
///      - Loading a UI layout from a file and showing it
///      - Subscribing to the UI layout's events
class SceneLoader : public Sample
{
    URHO3D_OBJECT(SceneLoader, Sample);

public:
    /// Construct.
    explicit SceneLoader(Context* context);

    /// Setup after engine initialization and before running the main loop.
    void Start() override;
    void Stop() override;

private:
    /// Construct the scene content.
    bool CreateScene();
    /// Construct user interface elements.
    void CreateUI();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();

    /// Subscribe to application-wide logic update event.
    void SubscribeToEvents();
    /// Reads input and moves the camera.
    void MoveCamera(float timeStep);

    // reload scene if filewatcher detected this
    void ReloadScene();

    // export components for use in blender
    void ExportComponents(const String& outputPaht);

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void HandleFileChanged(StringHash eventType, VariantMap& eventData);
    /// Handle reload start of the script file.
    void HandleScriptReloadStarted(StringHash eventType, VariantMap& eventData);
    /// Handle reload success of the script file.
    void HandleScriptReloadFinished(StringHash eventType, VariantMap& eventData);
    /// Handle reload failure of the script file.
    void HandleScriptReloadFailed(StringHash eventType, VariantMap& eventData);
    void HandleAfterRender(StringHash eventType, VariantMap& eventData);

    void HandleBlenderMSG(StringHash eventType, VariantMap& eventData);


    void HandleRequestFromBlender(const JSONObject &json);
    void UpdateCameras();
    void EnsureLight(Scene* scene);


    Scene* GetScene(const String& sceneName);
    ViewRenderer* GetViewRenderer(int viewId);
    ViewRenderer* CreateViewRenderer(Context* ctx, Scene* scene, int width, int height);
    void UpdateViewRenderer(ViewRenderer* renderer);

    void InitEditor();

    void ChangeFov(float delta);
  //  void CreateScreenshot();

  //  void HandleRequestFromBlender(const JSONObject& json);
//    void HandleRequestFromEngineToBlender();

    String sceneName;
    Vector<String> runtimeFlags;
    String exportPath;
    String additionalResourcePath;
    String customUI;

    int currentCamId;
    int showViewportId;
    bool updatedCamera;
    PODVector<Camera*> cameras;

    Camera* blenderViewportCamera;
    Node* blenderViewportCameraNode;

    bool editorVisible_;
    /// Script file.
    SharedPtr<ScriptFile> scriptFile_;
    float screenshotTimer;
    float screenshotInterval;
    bool automaticIntervallScreenshots;

    bool rtRenderRequested;
    RenderSurface* surface;
    SharedPtr<Texture2D> rtTexture;

    HashMap<StringHash,Scene*> scenes_;
    HashMap<int,ViewRenderer*> viewRenderers;
    HashSet<ViewRenderer*> updatedRenderers;
    ViewRenderer* currentViewRenderer;

    JSONFile jsonfile_;
};
