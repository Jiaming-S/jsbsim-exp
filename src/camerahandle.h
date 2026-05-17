#pragma once

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Platform/Sdl2Application.h>

#include "input/input.h"
#include "types/types.h"

using namespace Magnum::Math::Literals;
using namespace Magnum::Platform;

class CameraHandle {
  public:
    Magnum::SceneGraph::Camera3D *_camera;
    types::Object3D *_mount, *_revolut;

    CameraHandle() {};
    CameraHandle(types::Object3D* root, Magnum::Matrix4 projection_matrix);

    void reattach_to(types::Object3D* root);
    
    void apply_commanded_movement(
      input::CommandedMovement& commanded_movement,
      types::SimContext& sim_context
    );
};




