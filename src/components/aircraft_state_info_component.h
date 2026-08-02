#pragma once

#include "component.h"

#include "models/FGAerodynamics.h"
#include "models/FGPropagate.h"

class AircraftStateInfoComponent : public Component {
  public:
    explicit AircraftStateInfoComponent(std::shared_ptr<JSBSimExpBlackboard> bb, Sdl2Application *app) 
      : Component(std::move(bb), app) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
