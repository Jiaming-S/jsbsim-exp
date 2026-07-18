#include "input_component.h"

void InputComponent::init() {}

void InputComponent::quit() {}

void InputComponent::handle_dispatch() {
  using Magnum::Platform::Sdl2Application;

  // Pausing Toggle
  if (blackboard->keyboard_input_blackboard->keys_down[Magnum::Platform::Sdl2Application::Key::P]) {
    switch (blackboard->sim_state_blackboard->sim_physics_state) {
      case types::NORMAL: {
        blackboard->sim_state_blackboard->sim_physics_state = types::PAUSED;
        break;
      }
      case types::PAUSED: {
        blackboard->sim_state_blackboard->sim_physics_state = types::NORMAL;
        break;
      }
      case types::CUSTOM: {
        blackboard->sim_state_blackboard->sim_physics_state = types::NORMAL;
        break;
      }
    }
  }


  // Freecam Toggle
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::C]) {
    switch (blackboard->sim_state_blackboard->sim_control_type) {
      case types::CAMERA: {
        blackboard->sim_state_blackboard->sim_control_type = types::MODEL;
        break;
      }
      case types::MODEL: {
        blackboard->sim_state_blackboard->sim_control_type = types::CAMERA;
        break;
      }
    }
  }

  // Reset
  blackboard->keyboard_input_blackboard->commanded_translation = {0.0, 0.0, 0.0};
  blackboard->keyboard_input_blackboard->commanded_pitch = 0.0;
  blackboard->keyboard_input_blackboard->commanded_yaw = 0.0;
  blackboard->keyboard_input_blackboard->commanded_roll = 0.0;
  blackboard->keyboard_input_blackboard->mouse_delta = {0.0, 0.0};

  // Commanded Movement conversion (translation)
  // Translation forward/backward
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::W])
    blackboard->keyboard_input_blackboard->commanded_translation.z() -= 1.0f;
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::S])
    blackboard->keyboard_input_blackboard->commanded_translation.z() += 1.0f;

  // Translation left/right
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::A])
    blackboard->keyboard_input_blackboard->commanded_translation.x() -= 1.0f;
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::D])
    blackboard->keyboard_input_blackboard->commanded_translation.x() += 1.0f;
  
  // Translation up/down
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::Space])   
    blackboard->keyboard_input_blackboard->commanded_translation.y() += 1.0f;  
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::LeftCtrl])
    blackboard->keyboard_input_blackboard->commanded_translation.y() -= 1.0f;


  // Commanded Movement conversion (rotation)
  // Pitch up/down
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::Down])
    blackboard->keyboard_input_blackboard->commanded_pitch += 1.0f;
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::Up])  
    blackboard->keyboard_input_blackboard->commanded_pitch -= 1.0f;

  // Yaw left/right
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::Left]) 
    blackboard->keyboard_input_blackboard->commanded_yaw += 1.0f;
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::Right])
    blackboard->keyboard_input_blackboard->commanded_yaw -= 1.0f;

  // Roll right/left
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::Q])
    blackboard->keyboard_input_blackboard->commanded_roll += 1.0f;
  if (blackboard->keyboard_input_blackboard->keys_down[Sdl2Application::Key::E])
    blackboard->keyboard_input_blackboard->commanded_roll -= 1.0f;


  // Commanded Movement conversion (rotation)
  if (blackboard->sim_state_blackboard->sim_control_type == types::eSimControlType::MODEL) {
    blackboard->keyboard_input_blackboard->mouse_delta = Magnum::Vector2(
      blackboard->keyboard_input_blackboard->mouse_position -
      blackboard->keyboard_input_blackboard->prev_mouse_position
    );

    Magnum::Float yaw_movement = blackboard->keyboard_input_blackboard->mouse_delta.x() * blackboard->keyboard_input_blackboard->mouse_sensitivity;
    Magnum::Float pitch_movement = blackboard->keyboard_input_blackboard->mouse_delta.y() * blackboard->keyboard_input_blackboard->mouse_sensitivity;

    blackboard->keyboard_input_blackboard->commanded_yaw -= yaw_movement;
    blackboard->keyboard_input_blackboard->commanded_pitch -= pitch_movement;
  }


  // Populate blackboard prev values
  blackboard->keyboard_input_blackboard->prev_keys_down = blackboard->keyboard_input_blackboard->keys_down;
  blackboard->keyboard_input_blackboard->prev_mouse_position = blackboard->keyboard_input_blackboard->mouse_position;
}
