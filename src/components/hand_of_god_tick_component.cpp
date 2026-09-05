#include "hand_of_god_tick_component.h"

void HandOfGodTickComponent::init() {}

void HandOfGodTickComponent::quit() {}

void HandOfGodTickComponent::handle_dispatch() {
  // Clear blackboard
  blackboard->hand_of_god_blackboard->hog_controlled_aircraft_index = 0;
  blackboard->hand_of_god_blackboard->hog_yaw = 0.0;
  blackboard->hand_of_god_blackboard->hog_pitch = 0.0;
  blackboard->hand_of_god_blackboard->hog_roll = 0.0;
  blackboard->hand_of_god_blackboard->hog_throttle = 0.0;
  blackboard->hand_of_god_blackboard->hog_environment_controls.sim_pause = false;
  blackboard->hand_of_god_blackboard->hog_environment_controls.sim_resume = false;
  blackboard->hand_of_god_blackboard->hog_environment_controls.sim_step_physics_once = false;
  blackboard->hand_of_god_blackboard->hog_environment_controls.sim_reset_to_free_flight = false;

  // If hand of god disabled, don't try to recv and repopulate blackboard
  if (blackboard->sim_state_blackboard->hand_of_god_mode == types::eHandOfGodMode::HAND_OF_GOD_OFF) {
    return;
  }
  
  // Publish msgpack packed telemetry
  zmq::socket_t *hand_of_god_sub_socket = blackboard->network_blackboard->hand_of_god_sub_socket;
  zmq::message_t msg;
  
  // Recv msgpack hand of god command over zmq
  if (hand_of_god_sub_socket->recv(msg, zmq::recv_flags::dontwait)) {
    // Recv successful
    blackboard->hand_of_god_blackboard->unpack(static_cast<const char*>(msg.data()), msg.size());
    blackboard->sim_state_blackboard->hand_of_god_receipt_status = types::eHandOfGodReceiptStatus::PACKET_RECV_SUCCESS;
    blackboard->network_blackboard->hand_of_god_bytes_recvd = msg.size();
  }
  else {
    // Recv failed
    blackboard->sim_state_blackboard->hand_of_god_receipt_status = types::eHandOfGodReceiptStatus::PACKET_RECV_FAILURE;
    blackboard->network_blackboard->hand_of_god_bytes_recvd = 0;
  }
}
