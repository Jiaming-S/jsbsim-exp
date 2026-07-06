#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include "../types/types.h"
#include "blackboard.h"

class AircraftBlackboard : public Blackboard {
  public:
    AircraftBlackboard() {}

    // Inputs

    // Outputs
    types::eAircraftActive has_active_aircraft{types::eAircraftActive::NO_ACTIVE};
    uint32_t active_aircraft_index = 0;

    // Locals

};
