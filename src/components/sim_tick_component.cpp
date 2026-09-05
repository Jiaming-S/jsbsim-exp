#include "sim_tick_component.h"

void SimTickComponent::init() {}

void SimTickComponent::quit() {}

void SimTickComponent::handle_dispatch() {
  const types::eSimPhysicsState sim_physics_state = blackboard->sim_state_blackboard->sim_physics_state;
  const types::eSingleTickRequest single_tick_request = blackboard->sim_state_blackboard->single_tick_request;
  
  // Normal operation
  if (sim_physics_state == types::eSimPhysicsState::NORMAL) {
    // Tick all aircraft FDMExec's
    for (auto &ac : blackboard->aircraft_blackboard->aircraft) {
      ac._fdmexec->Run();
    }
  }

  // If we are in single-tick mode
  if (sim_physics_state == types::eSimPhysicsState::PAUSED && single_tick_request == types::eSingleTickRequest::REQUEST_TICK) {
    // Tick all aircraft FDMExec's
    for (auto &ac : blackboard->aircraft_blackboard->aircraft) {
      ac._fdmexec->Run();
    }

    // Debounce request
    blackboard->sim_state_blackboard->single_tick_request = types::eSingleTickRequest::NO_REQUEST;
  }
}
