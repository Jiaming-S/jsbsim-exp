#include "hand_of_god_tick_component.h"

void HandOfGodTickComponent::init() {}

void HandOfGodTickComponent::quit() {}

void HandOfGodTickComponent::handle_dispatch() {
  if (blackboard->sim_state_blackboard->hand_of_god_mode == types::eHandOfGodMode::HAND_OF_GOD_INACTIVE) {
    return;
  }

  { // Apply environment controls
    const types::EnvironmentControlsHandOfGod hog_environment_controls = blackboard->hand_of_god_blackboard->hog_environment_controls;
    if (hog_environment_controls.sim_reset) {
      
    }
    if (hog_environment_controls.sim_pause) {

    }
  }

  { // Apply control axis controls for specific aircraft


  }
}
