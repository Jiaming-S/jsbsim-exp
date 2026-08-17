#include "telemetry_tick_component.h"

void TelemetryTickComponent::init() {}

void TelemetryTickComponent::quit() {}

void TelemetryTickComponent::handle_dispatch() {
  const std::vector<AircraftHandle> &aircraft = blackboard->aircraft_blackboard->aircraft;
  for (auto& ac : aircraft) {
    blackboard->telemetry_blackboard->aircraft_handle_telemetry.state_info.push_back(ac._state_info);
  }

  const Magnum::Float commanded_aircraft_throttle = blackboard->input_blackboard->commanded_aircraft_throttle;
  const Magnum::Float commanded_aircraft_braking = blackboard->input_blackboard->commanded_aircraft_braking;
  const Magnum::Float commanded_aircraft_roll = blackboard->input_blackboard->commanded_aircraft_roll;
  const Magnum::Float commanded_aircraft_yaw = blackboard->input_blackboard->commanded_aircraft_yaw;
  const Magnum::Float commanded_aircraft_pitch = blackboard->input_blackboard->commanded_aircraft_pitch;
  blackboard->telemetry_blackboard->input_telemetry.commanded_aircraft_throttle = commanded_aircraft_throttle;
  blackboard->telemetry_blackboard->input_telemetry.commanded_aircraft_braking = commanded_aircraft_braking;
  blackboard->telemetry_blackboard->input_telemetry.commanded_aircraft_roll = commanded_aircraft_roll;
  blackboard->telemetry_blackboard->input_telemetry.commanded_aircraft_yaw = commanded_aircraft_yaw;
  blackboard->telemetry_blackboard->input_telemetry.commanded_aircraft_pitch = commanded_aircraft_pitch;

  blackboard->telemetry_blackboard->pack();
}
