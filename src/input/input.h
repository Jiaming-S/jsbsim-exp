#pragma once

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Platform/Sdl2Application.h>

#include <unordered_map>

#include "../types/types.h"

using namespace Magnum::Math::Literals;
using namespace Magnum::Platform;


namespace input {

struct CommandedMovement {
  Magnum::Vector3 translation{0.0f};
  Magnum::Float roll{0.0f};
  Magnum::Float yaw{0.0f};
  Magnum::Float pitch{0.0f};
  Magnum::Vector2 mouse_delta{0.0f};

  Magnum::Float& x;
  Magnum::Float& y;
  Magnum::Float& z;


  CommandedMovement()
   : x{translation.x()}, y{translation.y()}, z{translation.z()} {}
};

class GlobalInputHandler {
  public:
    std::unordered_map<Sdl2Application::Key, bool> _keys_down;
    
    Magnum::Vector2 _mouse_cur_position;
    Magnum::Vector2 _mouse_prev_position;
    float _mouse_sensitivity = 0.1f;
    bool _mouse_held = false;

    void handle_key_press_event(Sdl2Application::KeyEvent& event);
    void handle_key_release_event(Sdl2Application::KeyEvent& event);
    void handle_pointer_press_event(Sdl2Application::PointerEvent& event);
    void handle_pointer_release_event(Sdl2Application::PointerEvent& event);
    void handle_pointer_move_event(Sdl2Application::PointerMoveEvent& event);

    void mutate_sim_state(std::shared_ptr<types::SimContext> sim_context);
    
    CommandedMovement get_commanded_movement();
};

}
