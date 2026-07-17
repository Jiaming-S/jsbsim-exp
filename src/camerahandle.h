#pragma once

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Platform/Sdl2Application.h>

#include "types/types.h"

using namespace Magnum::Math::Literals;
using namespace Magnum::Platform;

class CameraHandle {
  public:
    Magnum::SceneGraph::Camera3D *_camera;
    types::Object3D *_mount, *_revolut;

    CameraHandle(types::Object3D* root, Magnum::Matrix4 projection_matrix);

    void reattach_to(types::Object3D* root);
};




