#include "sim_tick_component.h"

void SimTickComponent::init() {}

void SimTickComponent::quit() {}

void SimTickComponent::handle_dispatch() {
  // Tick all aircraft FDMExec's
  for (auto &ac : blackboard->aircraft_blackboard->aircraft) {
    ac._fdmexec->Run();
  }
}
