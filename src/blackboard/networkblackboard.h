#pragma once

#include "../types/types.h"
#include "blackboard.h"

class NetworkBlackboard : public Blackboard {
  public:
    NetworkBlackboard() {}

    // Inputs

    // Outputs
    zmq::context_t *zmq_ctx;
    zmq::socket_t *telemetry_pub_socket;

    // Locals

};
