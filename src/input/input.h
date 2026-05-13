#pragma once

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Platform/Sdl2Application.h>

#include <unordered_map>

#include "../types/types.h"

using namespace Magnum::Math::Literals;
using namespace Magnum::Platform;


namespace input {

struct CommandedMovement {
  Magnum::Vector3 translation;
  Magnum::Deg roll;
  Magnum::Deg yaw;
  Magnum::Deg pitch;
};

class GlobalInputHandler {
  std::unordered_map<Sdl2Application::Key, bool> _keys_down;
  
  Magnum::Vector2 _mouse_cur_position;
  Magnum::Vector2 _mouse_prev_position;
  bool _mouse_held = false;

  public:
    void handle_key_press_event(Sdl2Application::KeyEvent& event);
    void handle_key_release_event(Sdl2Application::KeyEvent& event);
    void handle_pointer_press_event(Sdl2Application::PointerEvent& event);
    void handle_pointer_release_event(Sdl2Application::PointerEvent& event);
    void handle_pointer_move_event(Sdl2Application::PointerMoveEvent& event);

    void mutate_sim_state(std::shared_ptr<types::SimContext> sim_context);
    
    CommandedMovement get_commanded_movement(
      float default_translation_speed = 0.1f,
      float shift_translation_speed_multiplier = 20.0f,
      float mouse_sensitivity = 0.2f,
      Magnum::Deg default_rotation_speed = 1.0_degf,
      float shift_rotation_speed_multiplier = 4.0f
    );
};

}
