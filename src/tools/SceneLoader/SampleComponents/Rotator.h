#pragma once

#include "Urho3D/Urho3DAll.h"

class Level;

class Rotator : public LogicComponent
{
  URHO3D_OBJECT(Rotator,LogicComponent);

public:
    explicit Rotator(Context* context);

    static void RegisterObject(Context* context);

    void Start() override;

    void Update(float timestep) override;

    inline void SetAxis(const Vector3& axis) { this->axis=axis;  }
    inline void SetSpeed(float speed) {  this->speed = speed; }

private:
    Vector3 axis;
    Vector3 rotateAxis;
    float speed;

};

