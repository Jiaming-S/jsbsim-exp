#include "hand_of_god_control_component.h"


void HandOfGodControlComponent::init() {}

void HandOfGodControlComponent::quit() {}

void HandOfGodControlComponent::handle_dispatch() {
  if (blackboard->sim_state_blackboard->hand_of_god_mode == types::eHandOfGodMode::HAND_OF_GOD_OFF) {
    return;
  }

  { // Apply environment controls
    const types::EnvironmentControlsHandOfGod hog_environment_controls = blackboard->hand_of_god_blackboard->hog_environment_controls;
    
    if (hog_environment_controls.sim_pause) {
      blackboard->sim_state_blackboard->sim_physics_state = types::eSimPhysicsState::PAUSED;
    }
    if (hog_environment_controls.sim_resume) {
      blackboard->sim_state_blackboard->sim_physics_state = types::eSimPhysicsState::NORMAL;
    }
    // Note: sim_tick_component runs on the same tick loop as hand_of_god_control_component
    if (hog_environment_controls.sim_step_physics_once) {
      // Force paused physics if not already
      blackboard->sim_state_blackboard->sim_physics_state = types::eSimPhysicsState::PAUSED;
      blackboard->sim_state_blackboard->single_tick_request = types::eSingleTickRequest::REQUEST_TICK;
    }
    if (hog_environment_controls.sim_reset_to_free_flight) {
      blackboard->sim_state_blackboard->scenario_reset_request = types::eScenarioResetRequest::REQUEST_RESET_TO_FREE_FLIGHT;
    }
  }

  { // Apply control axis controls for specific aircraft
    const float hog_yaw = blackboard->hand_of_god_blackboard->hog_yaw;
    const float hog_pitch = blackboard->hand_of_god_blackboard->hog_pitch;
    const float hog_roll = blackboard->hand_of_god_blackboard->hog_roll;
    const float hog_throttle = blackboard->hand_of_god_blackboard->hog_throttle;

    blackboard->input_blackboard->commanded_aircraft_throttle = hog_throttle;
    blackboard->input_blackboard->commanded_aircraft_roll = hog_roll;
    blackboard->input_blackboard->commanded_aircraft_yaw = hog_yaw;
    blackboard->input_blackboard->commanded_aircraft_pitch = hog_pitch;

    // Force target aircraft index
    const size_t hog_controlled_aircraft_index = blackboard->hand_of_god_blackboard->hog_controlled_aircraft_index;
    blackboard->aircraft_blackboard->active_aircraft_index = hog_controlled_aircraft_index;

    // Force active aircraft
    blackboard->sim_state_blackboard->has_active_aircraft = types::eAircraftActive::HAS_ACTIVE;
  }
}
