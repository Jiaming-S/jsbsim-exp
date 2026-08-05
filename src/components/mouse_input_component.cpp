#include "mouse_input_component.h"

void MouseInputComponent::init() {}

void MouseInputComponent::quit() {}

void MouseInputComponent::handle_dispatch() {
  using Magnum::Platform::Sdl2Application;

  const types::eSimControlType sim_control_type = blackboard->sim_state_blackboard->sim_control_type;
  const types::eCursorHidden cursor_hidden = blackboard->sim_state_blackboard->cursor_hidden;
  const Magnum::Vector2 mouse_delta = blackboard->input_blackboard->mouse_delta;
  const float mouse_sensitivity = blackboard->input_blackboard->mouse_sensitivity;

  // Perform commanded movement conversion (rotation)
  if (sim_control_type == types::eSimControlType::MODEL) {
    const Magnum::Float roll_movement = mouse_delta.x() * mouse_sensitivity;
    const Magnum::Float pitch_movement = mouse_delta.y() * mouse_sensitivity;

    blackboard->input_blackboard->commanded_roll -= roll_movement;
    blackboard->input_blackboard->commanded_pitch -= pitch_movement;
  }

  else if (sim_control_type == types::eSimControlType::CAMERA &&
           cursor_hidden == types::eCursorHidden::HIDDEN_AND_LOCKED) {
    const Magnum::Float yaw_movement = mouse_delta.x() * mouse_sensitivity;
    const Magnum::Float pitch_movement = mouse_delta.y() * mouse_sensitivity;

    blackboard->input_blackboard->commanded_yaw -= yaw_movement;
    blackboard->input_blackboard->commanded_pitch -= pitch_movement;
  }

  // Update blackboard prev values
  blackboard->input_blackboard->prev_mouse_position = blackboard->input_blackboard->mouse_position;
  blackboard->input_blackboard->mouse_delta = {0.0f, 0.0f};
}
