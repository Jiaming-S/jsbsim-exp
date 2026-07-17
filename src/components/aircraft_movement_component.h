#pragma once

#include "component.h"

class AircraftMovementComponent : public Component {
  public:
    explicit AircraftMovementComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
