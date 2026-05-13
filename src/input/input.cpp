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
  _mouse_cur_position = event.position(); 
  _mouse_prev_position = _mouse_cur_position;
}

void GlobalInputHandler::handle_pointer_release_event(Sdl2Application::PointerEvent& event) {
  _mouse_held = false;
}

void GlobalInputHandler::handle_pointer_move_event(Sdl2Application::PointerMoveEvent& event) {
  _mouse_cur_position = event.position();
}

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
}

CommandedMovement GlobalInputHandler::get_commanded_movement(
  float default_translation_speed, 
  float shift_translation_speed_multiplier,
  float mouse_sensitivity, 
  Magnum::Deg default_rotation_speed, 
  float shift_rotation_speed_multiplier
) {
  CommandedMovement cmd;

  float translation_speed = default_translation_speed;
  Magnum::Deg rotation_speed = default_rotation_speed;

  // Multipliers
  if (_keys_down[Sdl2Application::Key::LeftShift]) {
    translation_speed *= shift_translation_speed_multiplier;
    rotation_speed  *= shift_rotation_speed_multiplier;
  }

  // Translation
  if (_keys_down[Sdl2Application::Key::A]) cmd.translation.x() -= translation_speed;
  if (_keys_down[Sdl2Application::Key::D]) cmd.translation.x() += translation_speed;
  
  if (_keys_down[Sdl2Application::Key::LeftCtrl]) cmd.translation.y() -= translation_speed;
  if (_keys_down[Sdl2Application::Key::Space])    cmd.translation.y() += translation_speed;
  
  if (_keys_down[Sdl2Application::Key::W]) cmd.translation.z() -= translation_speed; // -Z is forward
  if (_keys_down[Sdl2Application::Key::S]) cmd.translation.z() += translation_speed;

  // Rotation
  if (_keys_down[Sdl2Application::Key::Right]) cmd.yaw -= rotation_speed;
  if (_keys_down[Sdl2Application::Key::Left])  cmd.yaw += rotation_speed;
  
  if (_keys_down[Sdl2Application::Key::Down]) cmd.pitch -= rotation_speed;
  if (_keys_down[Sdl2Application::Key::Up])   cmd.pitch += rotation_speed;

  if (_mouse_held) {
    Magnum::Vector2 mouse_delta = Magnum::Vector2(_mouse_cur_position - _mouse_prev_position) * mouse_sensitivity;
    cmd.yaw   -= Magnum::Deg(mouse_delta.x());
    cmd.pitch -= Magnum::Deg(mouse_delta.y());
    _mouse_prev_position = _mouse_cur_position; 
  }

  return cmd;
}



}

