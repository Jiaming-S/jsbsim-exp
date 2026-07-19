#include "keyboard_input_component.h"

void KeyboardInputComponent::init() {}

void KeyboardInputComponent::quit() {}

void KeyboardInputComponent::handle_dispatch() {
  using Magnum::Platform::Sdl2Application;
  
  const std::unordered_set<Sdl2Application::Key> keys_down = blackboard->keyboard_input_blackboard->keys_down;
  const types::eSimPhysicsState sim_physics_state = blackboard->sim_state_blackboard->sim_physics_state;
  const types::eSimControlType sim_control_type = blackboard->sim_state_blackboard->sim_control_type;
  const types::eCursorHidden cursor_hidden = blackboard->sim_state_blackboard->cursor_hidden;

  // Pausing Toggle
  if (keys_down.count(Sdl2Application::Key::P)) {
    switch (sim_physics_state) {
      case types::eSimPhysicsState::NORMAL: {
        blackboard->sim_state_blackboard->sim_physics_state = types::eSimPhysicsState::PAUSED;
        break;
      }
      case types::eSimPhysicsState::PAUSED: {
        blackboard->sim_state_blackboard->sim_physics_state = types::eSimPhysicsState::NORMAL;
        break;
      }
      case types::eSimPhysicsState::CUSTOM: {
        blackboard->sim_state_blackboard->sim_physics_state = types::eSimPhysicsState::NORMAL;
        break;
      }
    }
  }

  // Freecam Toggle
  if (keys_down.count(Sdl2Application::Key::C)) {
    switch (sim_control_type) {
      case types::eSimControlType::CAMERA: {
        blackboard->sim_state_blackboard->sim_control_type = types::eSimControlType::MODEL;
        break;
      }
      case types::eSimControlType::MODEL: {
        blackboard->sim_state_blackboard->sim_control_type = types::eSimControlType::CAMERA;
        break;
      }
    }
  }

  // Cursor visibility hold
  if (keys_down.count(Sdl2Application::Key::LeftAlt)) {
    blackboard->sim_state_blackboard->cursor_hidden = types::eCursorHidden::VISIBLE;
  }
  else {
    blackboard->sim_state_blackboard->cursor_hidden = types::eCursorHidden::HIDDEN_AND_LOCKED;
  }

  // Reset
  blackboard->keyboard_input_blackboard->commanded_translation = {0.0, 0.0, 0.0};
  blackboard->keyboard_input_blackboard->commanded_pitch = 0.0;
  blackboard->keyboard_input_blackboard->commanded_yaw = 0.0;
  blackboard->keyboard_input_blackboard->commanded_roll = 0.0;

  // Commanded Movement conversion (translation)
  // Translation forward/backward
  if (keys_down.count(Sdl2Application::Key::W)) blackboard->keyboard_input_blackboard->commanded_translation.z() -= 1.0f;
  if (keys_down.count(Sdl2Application::Key::S)) blackboard->keyboard_input_blackboard->commanded_translation.z() += 1.0f;

  // Translation left/right
  if (keys_down.count(Sdl2Application::Key::A)) blackboard->keyboard_input_blackboard->commanded_translation.x() -= 1.0f;
  if (keys_down.count(Sdl2Application::Key::D)) blackboard->keyboard_input_blackboard->commanded_translation.x() += 1.0f;
  
  // Translation up/down
  if (keys_down.count(Sdl2Application::Key::Space))    blackboard->keyboard_input_blackboard->commanded_translation.y() += 1.0f;  
  if (keys_down.count(Sdl2Application::Key::LeftCtrl)) blackboard->keyboard_input_blackboard->commanded_translation.y() -= 1.0f;


  // Commanded Movement conversion (rotation)
  // Pitch up/down
  if (keys_down.count(Sdl2Application::Key::Down)) blackboard->keyboard_input_blackboard->commanded_pitch += 1.0f;
  if (keys_down.count(Sdl2Application::Key::Up))   blackboard->keyboard_input_blackboard->commanded_pitch -= 1.0f;

  // Yaw left/right
  if (keys_down.count(Sdl2Application::Key::Left))  blackboard->keyboard_input_blackboard->commanded_yaw += 1.0f;
  if (keys_down.count(Sdl2Application::Key::Right)) blackboard->keyboard_input_blackboard->commanded_yaw -= 1.0f;

  // Roll right/left
  if (keys_down.count(Sdl2Application::Key::Q)) blackboard->keyboard_input_blackboard->commanded_roll += 1.0f;
  if (keys_down.count(Sdl2Application::Key::E)) blackboard->keyboard_input_blackboard->commanded_roll -= 1.0f;

  // Populate blackboard prev values
  // TODO: just do a .clear() and .insert() loop
  blackboard->keyboard_input_blackboard->prev_keys_down = keys_down;
}
