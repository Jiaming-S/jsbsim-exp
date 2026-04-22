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

    const float _mouse_sensitivity;
    Magnum::Vector2 _mouse_prev_position;
    bool _mouse_held = false;


    CameraHandle(float speed = 0.1f, Magnum::Deg rotation_speed = 1.0_degf, float mouse_sensitivity = 0.2f)
      : _camera{}, _default_speed{speed}, _default_rotation_speed{rotation_speed}, _mouse_sensitivity{mouse_sensitivity} {};

    void attach_to(types::Object3D* root, Magnum::Matrix4 projection_matrix);
    void handle_keypress(std::unordered_map<Sdl2Application::Key, bool>& keys_down);
    void handle_mouse_move(Magnum::Vector2 position);
    void set_held(Magnum::Vector2 position);
    void set_unheld();
};




