#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include "../types/types.h"
#include "blackboard.h"

class SimStateBlackboard : public Blackboard {
  public:
    SimStateBlackboard() {}

    // Inputs
    float camera_speed = 1.0f;
    float camera_speed_accelerated = 5.0f;

    // Outputs
    types::eSimPhysicsState sim_physics_state{types::eSimPhysicsState::NORMAL};
    types::eSimControlType sim_control_type{types::eSimControlType::CAMERA};
    types::eSimControlType sim_control_type_default{types::eSimControlType::CAMERA};
    
    types::eCameraType camera_type{types::eCameraType::FREE};
    
    types::eAircraftActive has_active_aircraft{types::eAircraftActive::NO_ACTIVE};

    types::eCursorHidden cursor_hidden{types::eCursorHidden::HIDDEN_AND_LOCKED};

    // Locals

};
