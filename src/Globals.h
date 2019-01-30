/// For stuff you want to share across game states.

#include <Urho3D/Urho3DAll.h>



class Globals
{
public:
    bool touchEnabled=false;

    Urho3D::Camera* camera=0;
    Urho3D::Scene* scene=0;
    Urho3D::ResourceCache* cache=0;
    Urho3D::Context* context=0;
    Urho3D::UIElement* ui_root=0;
    Urho3D::Engine* engine=0;
    Urho3D::PhysicsWorld* physical_world=0;
    Urho3D::DebugRenderer* debug_renderer=0;
    bool show_debug = true;


   // void AddGameState(game_state* gs);

 //   ~globals();

    /// Meyer Singleton
    static Globals* instance()
    {
        static Globals g;
        return &g;
    }

private:
    /// The current game states, so that game states can switch to another game state.
    /// Watch out that changing a game state will delete the current one.
    /// Also game states can be stacked on top of each other so they run parallel. This is used for the pause mode.

   // Urho3D::PODVector<game_state*> game_states;



};




