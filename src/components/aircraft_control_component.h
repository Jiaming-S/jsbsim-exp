#pragma once

#include "component.h"

class AircraftControlComponent : public Component {
  public:
    explicit AircraftControlComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
