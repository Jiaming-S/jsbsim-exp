#pragma once

#include "blackboard.h"
#include "../types/types.h"


class HandOfGodBlackboard : public SerializeMixin<HandOfGodBlackboard> {
  public:
    HandOfGodBlackboard() {}

    // Inputs
    size_t hog_controlled_aircraft_index = 0;
    float hog_yaw = 0.0;
    float hog_pitch = 0.0;
    float hog_roll = 0.0;
    float hog_throttle = 0.0;
    types::EnvironmentControlsHandOfGod hog_environment_controls;

    // Outputs

    // Local

    MSGPACK_DEFINE(
      hog_controlled_aircraft_index,
      hog_yaw,
      hog_pitch,
      hog_roll,
      hog_throttle,
      hog_environment_controls
    );
};
