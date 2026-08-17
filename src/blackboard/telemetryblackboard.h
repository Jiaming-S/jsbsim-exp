#pragma once

#include "blackboard.h"

#include "../types/types.h"
#include <vector>

class TelemetryBlackboard : public SendableBlackboard {
  public:
    TelemetryBlackboard() {}

    // Inputs
    struct AircraftHandleTelemetry {
      std::vector<types::AircraftStateInfo> state_info;
    } aircraft_handle_telemetry;

    struct InputTelemetry {
      Magnum::Float commanded_aircraft_throttle;
      Magnum::Float commanded_aircraft_braking;
      Magnum::Float commanded_aircraft_roll;
      Magnum::Float commanded_aircraft_yaw;
      Magnum::Float commanded_aircraft_pitch;
    } input_telemetry;

    // Outputs

    // Local

};
