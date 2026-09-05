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
    types::eSimControlType sim_control_type_default{types::eSimControlType::CAMERA};
    
    types::eCameraType camera_type{types::eCameraType::FREE};
    
    types::eAircraftActive has_active_aircraft{types::eAircraftActive::NO_ACTIVE};

    types::eCursorHidden cursor_hidden{types::eCursorHidden::HIDDEN_AND_LOCKED};

    types::eDebugViews debug_views{types::eDebugViews::DEBUG_NOT_VISIBLE};

    types::eTelemetryMode telemetry_mode{types::eTelemetryMode::TELEMETRY_ON};
    types::eTelemetryDeliveryStatus telemetry_delivery_status{types::eTelemetryDeliveryStatus::PACKET_NO_STATUS};

    types::eHandOfGodMode hand_of_god_mode{types::eHandOfGodMode::HAND_OF_GOD_OFF};
    types::eHandOfGodReceiptStatus hand_of_god_receipt_status{types::eHandOfGodReceiptStatus::PACKET_NO_STATUS};

    types::eScenarioResetRequest scenario_reset_request{types::eScenarioResetRequest::NO_REQUEST};

    types::eSingleTickRequest single_tick_request{types::eSingleTickRequest::NO_REQUEST};

    // Locals

};
