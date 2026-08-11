#pragma once

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Platform/Sdl2Application.h>

#include "types/types.h"

using namespace Magnum::Math::Literals;
using namespace Magnum::Platform;

class CameraHandle {
  public:
    types::Object3D *_mount, *_revolut;
    Magnum::SceneGraph::Camera3D *_camera;

    explicit CameraHandle(types::Object3D* root) :
      _mount{new types::Object3D{root}},
      _revolut{new types::Object3D{_mount}},
      _camera{new Magnum::SceneGraph::Camera3D{*_revolut}} {}

    CameraHandle& with_projection_matrix(Magnum::Matrix4 projection_matrix);
    CameraHandle& with_default_offset(types::eSimControlType camera_type);
    
    CameraHandle& attach_to(types::Object3D* root);
    CameraHandle& attach_to_scene_root(types::Object3D* scene_root);
};




