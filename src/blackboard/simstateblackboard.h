#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include "../types/types.h"
#include "blackboard.h"

class SimStateBlackboard : public Blackboard {
  public:
    SimStateBlackboard() {}

    // Inputs

    // Outputs
    types::eSimPhysicsState sim_physics_state{types::eSimPhysicsState::NORMAL};
    types::eSimControlType sim_control_type{types::eSimControlType::CAMERA};
    
    types::eCameraType camera_type{types::eCameraType::FREE};

    // Locals

};
