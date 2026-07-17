#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <vector>

#include "blackboard.h"
#include "../aircrafthandle.h"

class AircraftBlackboard : public Blackboard {
  public:
    AircraftBlackboard() {}

    // Inputs
    uint32_t active_aircraft_index = 0;

    // Outputs
    std::vector<AircraftHandle> aircraft;

    // Locals

};
