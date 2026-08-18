#pragma once

#include "blackboard.h"

class NetworkBlackboard : public Blackboard {
  public:
    NetworkBlackboard() {}

    // Inputs

    // Outputs
    zmq::context_t *zmq_ctx;
    zmq::socket_t *telemetry_pub_socket;
    zmq::socket_t *hand_of_god_sub_socket;

    // Locals

};
