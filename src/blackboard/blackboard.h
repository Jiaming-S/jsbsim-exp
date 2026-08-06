#pragma once

#include <zmq.hpp>
#include <msgpack.hpp>

class Blackboard {
  public:
    virtual ~Blackboard() = default;
};
