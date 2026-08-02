#include "vis_tick_component.h"

void VisTickComponent::init() {}

void VisTickComponent::quit() {}

void VisTickComponent::handle_dispatch() {
  // Tick all aircraft root objects
  for (auto& ac : blackboard->aircraft_blackboard->aircraft) {
    if (ac._aircraft_trail->size() > ac._aircraft_trail_size_limit) ac._aircraft_trail->pop_front();
    ac._aircraft_trail->push_back(types::AircraftTrailBreadcrumb{
      ac._state_info,
      ac._fdmexec->GetSimTime(),
    });
    
    ac._visual_root_object->resetTransformation()
      .rotateZ(-ac._state_info.roll)
      .rotateX( ac._state_info.pitch)
      .rotateY(-ac._state_info.yaw)
      .translate(utils::as_magnum_RUB(
        ac._state_info.north,
        ac._state_info.east,
        ac._state_info.down
      ));
  }
}
