#pragma once

#include "blackboard.h"

#include <vector>

class TelemetryBlackboard : public SerializeMixin<TelemetryBlackboard> {
  public:
    TelemetryBlackboard() {}

    struct AircraftHandleTelemetry {
      float pitch_rad;
      float roll_rad;
      float yaw_rad;
      float alpha_rad;
      double alt_ft;
      double north_ft;
      double east_ft;
      double down_ft;
      double north_spd_fps;
      double east_spd_fps;
      double down_spd_fps;
      double ground_spd_fps;
      double indicated_spd_fps;

      MSGPACK_DEFINE(
        pitch_rad,
        roll_rad,
        yaw_rad,
        alpha_rad,
        alt_ft,
        north_ft,
        east_ft,
        down_ft,
        north_spd_fps,
        east_spd_fps,
        down_spd_fps,
        ground_spd_fps,
        indicated_spd_fps
      );
    };

    struct InputTelemetry {
      float commanded_aircraft_throttle;
      float commanded_aircraft_braking;
      float commanded_aircraft_roll;
      float commanded_aircraft_yaw;
      float commanded_aircraft_pitch;

      MSGPACK_DEFINE(
        commanded_aircraft_throttle,
        commanded_aircraft_braking,
        commanded_aircraft_roll,
        commanded_aircraft_yaw,
        commanded_aircraft_pitch
      );
    };

    // Inputs

    // Outputs
    std::vector<AircraftHandleTelemetry> aircraft_handle_telemetry;
    InputTelemetry input_telemetry;

    // Local

    MSGPACK_DEFINE(
      aircraft_handle_telemetry,
      input_telemetry
    );
};
