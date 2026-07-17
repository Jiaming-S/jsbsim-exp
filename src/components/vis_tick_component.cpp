#include "vis_tick_component.h"

void VisTickComponent::init() {}

void VisTickComponent::quit() {}

void VisTickComponent::handle_dispatch() {
  // Tick all aircraft root objects
  for (auto& ac : blackboard->aircraft_blackboard_vec->aircraft) {
    types::AircraftStateInfo state = ac.as_aircraft_state();
    if (ac._aircraft_trail->size() > ac._aircraft_trail_size_limit) ac._aircraft_trail->pop_front();
    ac._aircraft_trail->push_back(types::AircraftTrailBreadcrumb{
      state,
      ac._fdmexec->GetSimTime(),
    });
    
    ac._visual_root_object->resetTransformation()
      .rotateZ(-state.roll)
      .rotateX( state.pitch)
      .rotateY(-state.yaw)
      .translate(utils::as_magnum_RUB(state.north, state.east, state.down));
  }
}
