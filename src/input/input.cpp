#include "input.h"

namespace input {

void GlobalInputHandler::handle_key_press_event(Sdl2Application::KeyEvent& event) {
  _keys_down[event.key()] = true;
}

void GlobalInputHandler::handle_key_release_event(Sdl2Application::KeyEvent& event) {
  _keys_down[event.key()] = false;
}

void GlobalInputHandler::handle_pointer_press_event(Sdl2Application::PointerEvent& event) {
  _mouse_held = true;
}

void GlobalInputHandler::handle_pointer_release_event(Sdl2Application::PointerEvent& event) {
  _mouse_held = false;
}

void GlobalInputHandler::handle_pointer_move_event(Sdl2Application::PointerMoveEvent& event) {
  _mouse_cur_position = event.position();
}

// TODO: move this all to a SimStateHandle owned by main app, not part of input
//   Input should be more loosely connected to sim state
void GlobalInputHandler::mutate_sim_state(std::shared_ptr<types::SimContext> sim_context) {
  if (_keys_down[Sdl2Application::Key::P]) {
    // Pressing P while NORMAL or CUSTOM will force to PAUSED
    if (sim_context->state == types::SimContext::State::NORMAL ||
        sim_context->state == types::SimContext::State::CUSTOM) {
      sim_context->state = types::SimContext::State::PAUSED;
    }
    // Pressing P while PAUSED will always go to NORMAL
    else if (sim_context->state == types::SimContext::State::PAUSED) {
      sim_context->state = types::SimContext::State::NORMAL;
    }
  }

  // Pressing C toggles FREECAM
  if (_keys_down[Sdl2Application::Key::C]) {
    if (sim_context->control_type == types::SimContext::ControlType::MODEL) {
      sim_context->control_type = types::SimContext::ControlType::CAMERA;
    }
    else {
      sim_context->control_type = types::SimContext::ControlType::MODEL;
    }
  }
}

CommandedMovement GlobalInputHandler::get_commanded_movement() {
  CommandedMovement cmd;

  // Translation
  if (_keys_down[Sdl2Application::Key::W]) cmd.z -= 1.0f;
  if (_keys_down[Sdl2Application::Key::S]) cmd.z += 1.0f;

  if (_keys_down[Sdl2Application::Key::A]) cmd.x -= 1.0f;
  if (_keys_down[Sdl2Application::Key::D]) cmd.x += 1.0f;
  
  if (_keys_down[Sdl2Application::Key::Space])    cmd.y += 1.0f;  
  if (_keys_down[Sdl2Application::Key::LeftCtrl]) cmd.y -= 1.0f;

  // Keyboard rotation
  if (_keys_down[Sdl2Application::Key::Down]) cmd.pitch += 1.0f;
  if (_keys_down[Sdl2Application::Key::Up])   cmd.pitch -= 1.0f;

  if (_keys_down[Sdl2Application::Key::Left])  cmd.yaw += 1.0f;
  if (_keys_down[Sdl2Application::Key::Right]) cmd.yaw -= 1.0f;

  if (_keys_down[Sdl2Application::Key::Q]) cmd.roll += 1.0f;
  if (_keys_down[Sdl2Application::Key::E]) cmd.roll -= 1.0f;

  // Mouse rotation
  if (_mouse_held) {
    Magnum::Vector2 mouse_delta = Magnum::Vector2(_mouse_cur_position - _mouse_prev_position);
    cmd.yaw   -= mouse_delta.x() * _mouse_sensitivity;
    cmd.pitch -= mouse_delta.y() * _mouse_sensitivity;
    cmd.mouse_delta = mouse_delta;
    _mouse_prev_position = _mouse_cur_position;
  }

  // Clamp values between -1.0 to 1.0 
  // TODO: roll/pitch/yaw might need to be clamped s.t. ||translation|| = 1.0f
  cmd.roll  = Magnum::Math::clamp(cmd.roll,  -1.0f, 1.0f);
  cmd.yaw   = Magnum::Math::clamp(cmd.yaw,   -1.0f, 1.0f);
  cmd.pitch = Magnum::Math::clamp(cmd.pitch, -1.0f, 1.0f);
  cmd.x = Magnum::Math::clamp(cmd.x, -1.0f, 1.0f);
  cmd.y = Magnum::Math::clamp(cmd.y, -1.0f, 1.0f);
  cmd.z = Magnum::Math::clamp(cmd.z, -1.0f, 1.0f);

  return cmd;
}



}

