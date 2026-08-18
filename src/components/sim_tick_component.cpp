#include "sim_tick_component.h"

void SimTickComponent::init() {}

void SimTickComponent::quit() {}

void SimTickComponent::handle_dispatch() {
  const types::eSimPhysicsState sim_physics_state = blackboard->sim_state_blackboard->sim_physics_state;
  if (sim_physics_state == types::eSimPhysicsState::NORMAL) {
    // Tick all aircraft FDMExec's
    for (auto &ac : blackboard->aircraft_blackboard->aircraft) {
      ac._fdmexec->Run();
    }
  }
}
