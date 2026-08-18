#pragma once

#include "blackboard.h"

#include <vector>

#include "../types/types.h"


class TelemetryBlackboard : public SerializeMixin<TelemetryBlackboard> {
  public:
    TelemetryBlackboard() {}

    // Inputs

    // Outputs
    std::vector<types::AircraftHandleTelemetry> aircraft_handle_telemetry;
    types::InputTelemetry input_telemetry;

    // Local

    MSGPACK_DEFINE(
      aircraft_handle_telemetry,
      input_telemetry
    );
};
