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

    const float SHIFT_SPEED_MULTIPLIER = 10.0f;
    const float SHIFT_ROTATION_SPEED_MULTIPLIER = 2.0f;

    const float _default_speed;
    const Magnum::Deg _default_rotation_speed;


    CameraHandle(float speed = 0.1f, Magnum::Deg rotation_speed = 1.0_degf)
      : _camera{}, _default_speed{speed}, _default_rotation_speed{rotation_speed} {};

    void attach_to(types::Object3D* root, Magnum::Matrix4 projection_matrix);
    void handle_keypress(std::unordered_map<Sdl2Application::Key, bool>& keys_down);
};




