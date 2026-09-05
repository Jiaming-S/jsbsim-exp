#include "keyboard_input_component.h"
#include "Magnum/Platform/Sdl2Application.h"

void KeyboardInputComponent::init() {}

void KeyboardInputComponent::quit() {}

bool is_edge(
  const Sdl2Application::Key key,
  const std::unordered_set<Sdl2Application::Key> keys_down,
  const std::unordered_set<Sdl2Application::Key> prev_keys_down
) {
  return (keys_down.count(key) && !prev_keys_down.count(key));
}

bool is_held(
  const Sdl2Application::Key key,
  const std::unordered_set<Sdl2Application::Key> keys_down
) {
  return keys_down.count(key);
}

void KeyboardInputComponent::handle_dispatch() {
  using Magnum::Platform::Sdl2Application;
  
  const std::unordered_set<Sdl2Application::Key> keys_down = blackboard->input_blackboard->keys_down;
  const std::unordered_set<Sdl2Application::Key> prev_keys_down = blackboard->input_blackboard->prev_keys_down;
  const types::eSimPhysicsState sim_physics_state = blackboard->sim_state_blackboard->sim_physics_state;
  const types::eSimControlType sim_control_type = blackboard->sim_state_blackboard->sim_control_type;
  const types::eSimControlType sim_control_type_default = blackboard->sim_state_blackboard->sim_control_type_default;
  
  const float camera_translation_speed_default = blackboard->camera_blackboard->camera_translation_speed_default;
  const float camera_translation_speed_accelerated = blackboard->camera_blackboard->camera_translation_speed_accelerated;

  // Pausing Toggle
  if (is_edge(Sdl2Application::Key::P, keys_down, prev_keys_down)) {
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

  // Freecam hold
  if (is_held(Sdl2Application::Key::C, keys_down)) {
    blackboard->sim_state_blackboard->sim_control_type = types::eSimControlType::CAMERA;
  }
  else {
    blackboard->sim_state_blackboard->sim_control_type = sim_control_type_default;
  }

  // Cursor visibility hold
  if (is_held(Sdl2Application::Key::LeftAlt, keys_down)) {
    blackboard->sim_state_blackboard->cursor_hidden = types::eCursorHidden::VISIBLE;
  }
  else {
    blackboard->sim_state_blackboard->cursor_hidden = types::eCursorHidden::HIDDEN_AND_LOCKED;
  }

  // Camera speed hold
  if (is_held(Sdl2Application::Key::LeftShift, keys_down)) {
    blackboard->camera_blackboard->camera_translation_speed = camera_translation_speed_accelerated;
  }
  else {
    blackboard->camera_blackboard->camera_translation_speed = camera_translation_speed_default;
  }

  // Don't reset aircraft commands if hand of god is commanding
  if (blackboard->sim_state_blackboard->hand_of_god_mode != types::eHandOfGodMode::HAND_OF_GOD_SCRIPT_CONTROL) {
    blackboard->input_blackboard->commanded_aircraft_throttle = 0.0;
    blackboard->input_blackboard->commanded_aircraft_braking = 0.0;
    blackboard->input_blackboard->commanded_aircraft_pitch = 0.0;
    blackboard->input_blackboard->commanded_aircraft_yaw = 0.0;
    blackboard->input_blackboard->commanded_aircraft_roll = 0.0;
  }

  { // Reset camera commands 
    blackboard->input_blackboard->commanded_camera_translation = {0.0, 0.0, 0.0};
    blackboard->input_blackboard->commanded_camera_pitch = 0.0;
    blackboard->input_blackboard->commanded_camera_yaw = 0.0;
    blackboard->input_blackboard->commanded_camera_roll = 0.0;
  }

  // Controlling camera
  if (sim_control_type == types::eSimControlType::CAMERA) {
    // Commanded Movement conversion (translation)
    // Translation forward/backward
    if (keys_down.count(Sdl2Application::Key::W)) blackboard->input_blackboard->commanded_camera_translation.z() -= 1.0f;
    if (keys_down.count(Sdl2Application::Key::S)) blackboard->input_blackboard->commanded_camera_translation.z() += 1.0f;

    // Translation left/right
    if (keys_down.count(Sdl2Application::Key::A)) blackboard->input_blackboard->commanded_camera_translation.x() -= 1.0f;
    if (keys_down.count(Sdl2Application::Key::D)) blackboard->input_blackboard->commanded_camera_translation.x() += 1.0f;
    
    // Translation up/down
    if (keys_down.count(Sdl2Application::Key::Space))    blackboard->input_blackboard->commanded_camera_translation.y() += 1.0f;  
    if (keys_down.count(Sdl2Application::Key::LeftCtrl)) blackboard->input_blackboard->commanded_camera_translation.y() -= 1.0f;

    // Commanded Movement conversion (rotation)
    // Pitch up/down
    if (keys_down.count(Sdl2Application::Key::Down)) blackboard->input_blackboard->commanded_camera_pitch += 1.0f;
    if (keys_down.count(Sdl2Application::Key::Up))   blackboard->input_blackboard->commanded_camera_pitch -= 1.0f;

    // Yaw left/right
    if (keys_down.count(Sdl2Application::Key::Left))  blackboard->input_blackboard->commanded_camera_yaw += 1.0f;
    if (keys_down.count(Sdl2Application::Key::Right)) blackboard->input_blackboard->commanded_camera_yaw -= 1.0f;

    // Roll left/right
    if (keys_down.count(Sdl2Application::Key::Q)) blackboard->input_blackboard->commanded_camera_roll += 1.0f;
    if (keys_down.count(Sdl2Application::Key::E)) blackboard->input_blackboard->commanded_camera_roll -= 1.0f;
  }

  // Controlling model
  else if (sim_control_type == types::eSimControlType::MODEL) {
    // Commanded Movement conversion (translation)
    // Translation forward/backward
    if (keys_down.count(Sdl2Application::Key::LeftShift)) blackboard->input_blackboard->commanded_aircraft_throttle += 1.0f;
    if (keys_down.count(Sdl2Application::Key::LeftCtrl))  blackboard->input_blackboard->commanded_aircraft_throttle -= 1.0f;
    if (keys_down.count(Sdl2Application::Key::LeftCtrl))  blackboard->input_blackboard->commanded_aircraft_braking += 1.0f;

    // Commanded Movement conversion (rotation)
    // Pitch up/down
    if (keys_down.count(Sdl2Application::Key::S)) blackboard->input_blackboard->commanded_aircraft_pitch += 1.0f;
    if (keys_down.count(Sdl2Application::Key::W)) blackboard->input_blackboard->commanded_aircraft_pitch -= 1.0f;

    // Yaw left/right
    if (keys_down.count(Sdl2Application::Key::E)) blackboard->input_blackboard->commanded_aircraft_yaw += 1.0f;
    if (keys_down.count(Sdl2Application::Key::Q)) blackboard->input_blackboard->commanded_aircraft_yaw -= 1.0f;

    // Roll left/right
    if (keys_down.count(Sdl2Application::Key::A)) blackboard->input_blackboard->commanded_aircraft_roll += 1.0f;
    if (keys_down.count(Sdl2Application::Key::D)) blackboard->input_blackboard->commanded_aircraft_roll -= 1.0f;
  }

  // Populate blackboard prev values
  // TODO: just do a .clear() and .insert() loop
  blackboard->input_blackboard->prev_keys_down = keys_down;
}
