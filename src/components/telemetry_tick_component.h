#pragma once

#include <msgpack.hpp>
#include <zmq.hpp>

#include "component.h"

class TelemetryTickComponent : public Component {
  public:
    explicit TelemetryTickComponent(std::shared_ptr<JSBSimExpBlackboard> bb, Sdl2Application *app) 
      : Component(std::move(bb), app) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
