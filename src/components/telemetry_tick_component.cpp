#include "telemetry_tick_component.h"

void TelemetryTickComponent::init() {}

void TelemetryTickComponent::quit() {}

void TelemetryTickComponent::handle_dispatch() {
  if (blackboard->sim_state_blackboard->telemetry_mode != types::eTelemetryMode::TELEMETRY_ON) {
    blackboard->sim_state_blackboard->telemetry_delivery_status = types::eTelemetryDeliveryStatus::PACKET_NO_STATUS;
    blackboard->network_blackboard->telemetry_bytes_sent = 0;
    return;
  }

  { // Telemetry for each Aircraft Handle's state_info
    const std::vector<AircraftHandle> &aircraft = blackboard->aircraft_blackboard->aircraft;
    blackboard->telemetry_blackboard->aircraft_handle_telemetry.clear();

    for (auto& ac : aircraft) {
      blackboard->telemetry_blackboard->aircraft_handle_telemetry.push_back(
        types::AircraftHandleTelemetry{
          float(ac._state_info.pitch_rad),
          float(ac._state_info.roll_rad),
          float(ac._state_info.yaw_rad),
          float(ac._state_info.alpha_rad),
          ac._state_info.alt_ft,
          ac._state_info.north_ft,
          ac._state_info.east_ft,
          ac._state_info.down_ft,
          ac._state_info.north_spd_fps,
          ac._state_info.east_spd_fps,
          ac._state_info.down_spd_fps,
          ac._state_info.ground_spd_fps,
          ac._state_info.indicated_spd_fps,
        }
      );
    }
  }

  { // Telemetry for each commanded_aircraft_throttle/braking/yaw/pitch/roll command
    blackboard->telemetry_blackboard->input_telemetry = types::InputTelemetry {
      blackboard->input_blackboard->commanded_aircraft_throttle,
      blackboard->input_blackboard->commanded_aircraft_braking,
      blackboard->input_blackboard->commanded_aircraft_roll,
      blackboard->input_blackboard->commanded_aircraft_yaw,
      blackboard->input_blackboard->commanded_aircraft_pitch
    };
  }

  { // Telemetry for sim state
    const bool sim_paused = blackboard->sim_state_blackboard->sim_physics_state == types::eSimPhysicsState::NORMAL;
    
    blackboard->telemetry_blackboard->environment_telemetry = types::EnvironmentTelemetry {
      sim_paused
    };
  }

  { // Publish msgpack packed telemetry
    zmq::socket_t *telemetry_pub_socket = blackboard->network_blackboard->telemetry_pub_socket;
    msgpack::sbuffer sbuf = blackboard->telemetry_blackboard->pack();
    zmq::message_t msg{sbuf.data(), sbuf.size()};
    
    // Send msgpack telemetry over zmq
    if (telemetry_pub_socket->send(msg, zmq::send_flags::none)) {
      // Send successful
      blackboard->sim_state_blackboard->telemetry_delivery_status = types::eTelemetryDeliveryStatus::PACKET_SEND_SUCCESS;
      blackboard->network_blackboard->telemetry_bytes_sent = sbuf.size();
    }
    else {
      // Send failed
      blackboard->sim_state_blackboard->telemetry_delivery_status = types::eTelemetryDeliveryStatus::PACKET_SEND_FAILURE;
      blackboard->network_blackboard->telemetry_bytes_sent = 0;
    }
  }
}
