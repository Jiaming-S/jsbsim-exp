#pragma once

#include "component.h"

class SimTickComponent : public Component {
  public:
    explicit SimTickComponent(std::shared_ptr<JSBSimExpBlackboard> bb, Sdl2Application *app) 
      : Component(std::move(bb), app) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
