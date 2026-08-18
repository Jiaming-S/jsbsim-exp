#include "telemetry_tick_component.h"

void TelemetryTickComponent::init() {}

void TelemetryTickComponent::quit() {}

void TelemetryTickComponent::handle_dispatch() {
  { // Telemetry for each Aircraft Handle's state_info
    const std::vector<AircraftHandle> &aircraft = blackboard->aircraft_blackboard->aircraft;
    blackboard->telemetry_blackboard->aircraft_handle_telemetry.clear();

    for (auto& ac : aircraft) {
      const Magnum::Rad pitch_rad = ac._state_info.pitch_rad;
      const Magnum::Rad roll_rad = ac._state_info.roll_rad;
      const Magnum::Rad yaw_rad = ac._state_info.yaw_rad;
      const Magnum::Rad alpha_rad = ac._state_info.alpha_rad;
      const double alt_ft = ac._state_info.alt_ft;
      const double north_ft = ac._state_info.north_ft;
      const double east_ft = ac._state_info.east_ft;
      const double down_ft = ac._state_info.down_ft;
      const double north_spd_fps = ac._state_info.north_spd_fps;
      const double east_spd_fps = ac._state_info.east_spd_fps;
      const double down_spd_fps = ac._state_info.down_spd_fps;
      const double ground_spd_fps = ac._state_info.ground_spd_fps;
      const double indicated_spd_fps = ac._state_info.indicated_spd_fps;

      blackboard->telemetry_blackboard->aircraft_handle_telemetry.push_back(types::AircraftHandleTelemetry{
        float(pitch_rad),
        float(roll_rad),
        float(yaw_rad),
        float(alpha_rad),
        alt_ft,
        north_ft,
        east_ft,
        down_ft,
        north_spd_fps,
        east_spd_fps,
        down_spd_fps,
        ground_spd_fps,
        indicated_spd_fps,
      });
    }
  }

  { // Telemetry for each commanded_aircraft_throttle/braking/yaw/pitch/roll command
    const Magnum::Float commanded_aircraft_throttle = blackboard->input_blackboard->commanded_aircraft_throttle;
    const Magnum::Float commanded_aircraft_braking = blackboard->input_blackboard->commanded_aircraft_braking;
    const Magnum::Float commanded_aircraft_roll = blackboard->input_blackboard->commanded_aircraft_roll;
    const Magnum::Float commanded_aircraft_yaw = blackboard->input_blackboard->commanded_aircraft_yaw;
    const Magnum::Float commanded_aircraft_pitch = blackboard->input_blackboard->commanded_aircraft_pitch;
    
    blackboard->telemetry_blackboard->input_telemetry = types::InputTelemetry {
      commanded_aircraft_throttle,
      commanded_aircraft_braking,
      commanded_aircraft_roll,
      commanded_aircraft_yaw,
      commanded_aircraft_pitch
    };
  }

  blackboard->telemetry_blackboard->pack();
}
